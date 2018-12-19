#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/*Linha de compilação no terminal:
gcc Main.c -o huffman
Executar:
Para compactação: ./nome programa (nome do arquivo a ser codificado) (nome do arquivo compatado a ser salvo)
Para descompactação: ./nome programa (nome do arquivo a ser descompactado)
*/

#include "funcoesCodificacaoDeHuffman.h"

#define CRIAR_NO_CARACTER 1
#define NAO_CRIAR_NO_CARACTER 0

#define CARACTERS_ASCII 256

#define TAM_BUFFER 8

int main(int argc, char** argv)
{
    int caracteresPresentes = 0,i,j;
    unsigned char caracter;
    LISTA_DUPLAMENTE_ENCADEADA_ORDENADA *lista_nos = criar_lista();
    int caractersASCII[CARACTERS_ASCII] = {0};

    /*===========================COMPACTAÇÃO==========================================*/
    /*Se temos dois argumentos de entrada e se forem escritos na ordem correta como mostrado 
    no início do código,teremos a compactação.*/
    if(argc == 3) 
    {
        FILE *arquivoOriginal = fopen(argv[1],"r+"); //Abre o arquivo de texto para leitura e compactação.
        FILE *arquivoCompactado = fopen(argv[2], "w+b");//Cria um arquivo para receber a compactação(escrita nesse arquivo).

        //Tratamento de erros para a manipulação dos arquivos.
        if(arquivoOriginal == NULL)
        {
            printf("O arquivo de origem nao encontrado.\n");
            return -1;
        }

        if(arquivoCompactado == NULL)
        {
            printf("O arquivo para compactar nao criado.\n");
            return -1;
        }

        /*============Primeira parte: Identificar os caracteres do texto e a quantidade de cada um.==================*/

        //Calcula as frequências de cada letra presente no texto.
        while(!feof(arquivoOriginal))
        {
            fread(&caracter, sizeof(unsigned char),1,arquivoOriginal);
            caractersASCII[caracter]++;
        }

        //Volta para o início do arquivo(início do texto).
        rewind(arquivoOriginal);

        //Irá criar um nó para os tipos de caracteres que compões o texto.
        NO* aux;

        for(i = 0;i < CARACTERS_ASCII;i++)
        {
            if(caractersASCII[i] != 0)
            {
                aux = criar_no((unsigned char)i,caractersASCII[i],CRIAR_NO_CARACTER);
                inserirListaOrdenada(lista_nos,aux);
                caracteresPresentes++;
            }
        }

        /*================Segundo passo: Construir a arvore de Huffman==================================*/

        //Execução do algoritmo da construção da árvore binária de huffman: usar uma lista ordenada contendo inicialmente os nós 
        //dos caracteres do texto juntamente com suas frequências.

        while(lista_nos->tamanho != 1)
        {
            //Cria o novo nó que será inserido na lista.
            aux = criar_no(NAO_CRIAR_NO_CARACTER,somarFrequencia(lista_nos->inicio,lista_nos->inicio->proximo),NAO_CRIAR_NO_CARACTER);
            //Os filhos do novo nó serão os dois nós iniciais da lista ordenada(com menores frequências).
            aux->filhoesquerdo = lista_nos->inicio;
            aux->filhodireito = lista_nos->inicio->proximo;
            //Irá remover os dois primeiros nós que estavam na lista.
            removerListaHuffman(lista_nos);
            //Insere o novo nó na lista ordenada.
            inserirListaOrdenada(lista_nos,aux);
        }

        /*O nó que sobrar na lista será o nó com o somatório total das frequencias dos caracteres do texto e
        consequentemente será o inicio da lista duplamente encadeada unitaria.*/
        ARVORE_BINARIA *arvoreDeHuffman = criar_arvore(lista_nos->inicio);
        int totalCaracteres = lista_nos->inicio->frequencia;

        //Apaga da memória a estrutura de lista que não será mais necessária.
        free(lista_nos);

        /*============Terceiro passo: Montar a nova sequencia de bits e a tabela de codificacao.===================*/

        //Valor que recebe a maior quantidade de bits de uma sequência formada.Isso é dado pela altura da árvore.
        int maiorSequenciaBits = alturaArvoreBinaria(arvoreDeHuffman->raiz);
        //Quantos buffers de 8 bits(1 byte) serão necessários para representa a maior sequência de bits.
        int quantidadeDeBuffersNecessarios = maiorSequenciaBits / 8;
        //Se não der um valor exato, a paroximação desse valor sempre considera o próximo inteiro maior que o resultado.
        if(maiorSequenciaBits % 8 != 0)
            quantidadeDeBuffersNecessarios++;

        //Irá guardar os bits lidos no deslocamento na árvore para identificar cada sequência correspondente ao seu caracter.
        short int *sequenciaBits = (short int*) malloc(maiorSequenciaBits * sizeof(short int));
        //Guardará o endereço para os nós folhas da árvore(os nós que contem os caracteres).
        NO **nos_folhas = (NO**) malloc(caracteresPresentes * sizeof(NO*));

        //Contador do índice do vetor de ponteiros "nos_folhas".
        int aux_qtdAdcionadosNoVetor = 0;
        //Função que obtem a sequência de cada caracter.
        posordem_sequencia_bitsValidos(arvoreDeHuffman->raiz,0,sequenciaBits,caracteresPresentes,nos_folhas,quantidadeDeBuffersNecessarios, &aux_qtdAdcionadosNoVetor);

        free(sequenciaBits);

        /*=================Quarto passo: Escrever as informações no arquivo compactado.=============================*/

        unsigned char numCaracteresPresentes = 0;
        unsigned char numBuffersNecessarios = 0;//até 256 buffers
        int numero_aux = caracteresPresentes, bit_aux,bits_escritos = 0;

        //While do numero de caracteres do texto.
        //Simula o algortimo para converter um número em binário.
        while(numero_aux != 0) //quando numero_aux == 1, 1 / 2 == 0.5 ~= 0 (pois numero_aux é int).
        {
            bit_aux = conversorParaBinario(numero_aux);
            numCaracteresPresentes = escreverBitInvertido(bit_aux,numCaracteresPresentes);
            bits_escritos++;
            numero_aux /= 2;
        }

        //Para guardar o número em binário.
        numCaracteresPresentes >>= (TAM_BUFFER - bits_escritos);

        numero_aux = quantidadeDeBuffersNecessarios;
        bits_escritos = 0;

        //While do numero de buffers usados para a maior sequencia de bits.
        while(numero_aux != 0)
        {
            bit_aux = conversorParaBinario(numero_aux);
            numBuffersNecessarios = escreverBitInvertido(bit_aux,numBuffersNecessarios);
            bits_escritos++;
            numero_aux /= 2;
        }

        numBuffersNecessarios >>= (TAM_BUFFER - bits_escritos);

        //---------------Início da escrita efetiva no arquivo compactado.------------------------------


        fwrite(&numBuffersNecessarios,sizeof(unsigned char),1,arquivoCompactado);
        fwrite(&numCaracteresPresentes,sizeof(unsigned char),1,arquivoCompactado);
        fwrite(&totalCaracteres,sizeof(int),1,arquivoCompactado);



        //-------Tabela de caracteres: caracter ||| sequencia ||| bits validos da sequencia.-------------------

        unsigned char auxc;
        unsigned char buffer_aux;

        for(i = 0;i < caracteresPresentes;i++)
        {
            auxc = nos_folhas[i]->caracter;
            fwrite(&auxc,sizeof(unsigned char),1,arquivoCompactado);

            for(j = 0;j < numBuffersNecessarios;j++)
            {
                buffer_aux = nos_folhas[i]->sequenciaBits[j];
                fwrite(&buffer_aux,sizeof(unsigned char),1,arquivoCompactado);
            }

            auxc = nos_folhas[i]->bitsValidos;
            fwrite(&auxc,sizeof(unsigned char),1,arquivoCompactado);
        }


        //---------------------Escrita do texto compactado.-------------------------------------------

        codificar(nos_folhas,caracteresPresentes,arquivoOriginal,arquivoCompactado);

        /*=======================Quinto passo: Calcular a taxa de compressão======================*/

        float taxaCompressao;
        int totalBitsUsados = 0;

        for(i = 0;i < caracteresPresentes;i++)
        {
            totalBitsUsados += caractersASCII[nos_folhas[i]->caracter] * (int)nos_folhas[i]->bitsValidos;
        }

        taxaCompressao = 1 - (totalBitsUsados / (totalCaracteres * 8.0));
        taxaCompressao *= 100; //Porcentagem do valor.

        printf("Arquivo compactado com sucesso.\nTaxa de Compressão de %.2f porcento.\n",taxaCompressao);

        fclose(arquivoOriginal);
        fclose(arquivoCompactado);
        apagar_arvore(arvoreDeHuffman);
    }
    else if(argc == 2)
    {

        FILE *arquivoCompactado = fopen(argv[1],"rb");//Arquivo compactada que será lido para a descompactação.
        FILE *arquivoDescompactado = fopen("TextoDescompactado.txt","w+"); //Arquivo criado para receber o texto descompactado.

        if(arquivoCompactado == NULL)
        {
            printf("O arquivo compactado nao encontrado.\n");
            return -1;
        }

        if(arquivoDescompactado == NULL)
        {
            printf("O arquivo para descompacatar nao criado.\n");
            return -1;
        }


        /*============Primeira parte: Lendo os tres parametros iniciais do arquivo compactado==============*/

        int numBuffersNecessarios, numCaracteresPresentes,numTotalCaracteres;
        unsigned char tiposCaracteres, buffersNecessarios;

        fread(&buffersNecessarios,sizeof(unsigned char),1,arquivoCompactado);
        fread(&tiposCaracteres,sizeof(unsigned char),1,arquivoCompactado);
        fread(&numTotalCaracteres,sizeof(int),1,arquivoCompactado);

        numBuffersNecessarios = (int)buffersNecessarios;
        numCaracteresPresentes = (int)tiposCaracteres;

        TABELA *tabelaCodificacao = (TABELA*)malloc(numCaracteresPresentes * sizeof(TABELA));
        unsigned char auxc;

        /*===================Segunda parte: Lendo a tabela de caracteres.=====================*/


        for(i = 0;i < numCaracteresPresentes;i++)
        {
            tabelaCodificacao[i].sequencia = (unsigned char*)malloc(numBuffersNecessarios * sizeof(unsigned char));

            fread(&auxc,sizeof(unsigned char),1,arquivoCompactado);
            tabelaCodificacao[i].caracter = auxc;

            for(j = 0;j < numBuffersNecessarios;j++)
            {
                fread(&auxc,sizeof(unsigned char),1,arquivoCompactado);
                tabelaCodificacao[i].sequencia[j] = auxc;
            }

            fread(&auxc,sizeof(unsigned char),1,arquivoCompactado);
            tabelaCodificacao[i].bitsValidos = auxc;
        }

        /*===================Terceira parte: Montando a arvore de Huffman================*/

        NO *raiz = criar_no(0,numTotalCaracteres,0);

        ARVORE_BINARIA *arvoreDeHuffman = criar_arvore(raiz);

        construirHuffman(arvoreDeHuffman,tabelaCodificacao,numCaracteresPresentes);

        /*=======================Quata parte: Decodificar a mensagem codificada.====================*/

        decodificar(arvoreDeHuffman,numTotalCaracteres,arquivoCompactado,arquivoDescompactado);

        printf("Arquivo descompactado com sucesso.\n");

        apagar_arvore(arvoreDeHuffman);
        free(tabelaCodificacao);
        fclose(arquivoCompactado);
        fclose(arquivoDescompactado);
    }
    else
    {
        printf("Parâmetros de função inválidos.\n");
    }

    return 0;
}
