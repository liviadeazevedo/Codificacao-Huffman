#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define TRUE 1;
#define FALSE 0;

#define TAM_BUFFER 8

//Struct do nó que será usado para a árvore.
typedef struct NO{

    int frequencia;
    unsigned char caracter;
    struct NO *filhoesquerdo;
    struct NO *filhodireito;
    struct NO *proximo;
    struct NO *anterior;
    unsigned char* sequenciaBits;
    unsigned char bitsValidos; //representa números de 0 a 255.

}NO;

//Struct que simula a tabela das informações da compactação.
typedef struct{

    unsigned char caracter;
    unsigned char *sequencia;
    unsigned char bitsValidos;

}TABELA;

//Árvore binária.
typedef struct ARVORE_BINARIA {

	NO *raiz;

}ARVORE_BINARIA;

//Lista duplamente encadeada ordenada.
typedef struct {

    NO *inicio;
    NO *fim;
    int tamanho;

}LISTA_DUPLAMENTE_ENCADEADA_ORDENADA;

/*==================================LISTA DUPLAMENTE ENCADEADA ORDENADA================================*/
LISTA_DUPLAMENTE_ENCADEADA_ORDENADA* criar_lista(){

	LISTA_DUPLAMENTE_ENCADEADA_ORDENADA* nova_lista = (LISTA_DUPLAMENTE_ENCADEADA_ORDENADA*) malloc(sizeof(LISTA_DUPLAMENTE_ENCADEADA_ORDENADA));

	if(nova_lista != NULL){
		nova_lista->inicio = NULL;
		nova_lista->fim = NULL;
		nova_lista->tamanho = 0;
	}

	return nova_lista;
};

int listaVazia(LISTA_DUPLAMENTE_ENCADEADA_ORDENADA *lista)
{
    if(lista->inicio == NULL) //ou lista->fim == NULL, poderia ser um das duas condições
        return TRUE;

    if(lista->inicio != NULL)
        return FALSE;
};

NO* criar_no(unsigned char caracter,int frequencia,int criar_no_caracter)
{
    NO* novo_no = (NO*) malloc(sizeof(NO));

    if(criar_no_caracter)
    {
        novo_no->caracter = caracter;
        novo_no->frequencia = frequencia;
        novo_no->anterior = NULL;
        novo_no->proximo = NULL;
        novo_no->filhoesquerdo = NULL;
        novo_no->filhodireito = NULL;
        novo_no->bitsValidos = 0;
    }
    else
    {
        novo_no->frequencia = frequencia;
        novo_no->anterior = NULL;
        novo_no->proximo = NULL;
        novo_no->filhoesquerdo = NULL;
        novo_no->filhodireito = NULL;
        novo_no->bitsValidos = 0;
    }

    return novo_no;
};

int inserirListaOrdenada(LISTA_DUPLAMENTE_ENCADEADA_ORDENADA *lista, NO* no)
{
    NO* aux = (NO*) malloc(sizeof(NO));

    aux = lista->inicio;

    while(aux != NULL) //Se aux->proximo == NULL, quer dizer que chegamos ao ultimo elemento da lista.
    {
        if(aux->frequencia >= no->frequencia)
        {
            no->proximo = aux;
            no->anterior = aux->anterior;
            aux->anterior = no;

            if(no->anterior != NULL)
                no->anterior->proximo = no;
            else
                lista->inicio = no;

            lista->tamanho++;
            return TRUE;
        }
        else
        {
            aux = aux->proximo;
        }
    }
    //Se saiu do while, quer dizer que o no->frequencia é maior que todos os valores da lista ou a lista
    //está vazia. Inserirmos então o elemento no fim da lista.

    if(!listaVazia(lista))
    {
        aux = lista->fim;
        lista->fim->proximo = no;
        no->anterior = lista->fim;
        no->proximo = NULL;
        lista->fim = no;
        lista->tamanho++;
        return TRUE;
    }
    else //Se lista->fim == NULL, a lista está vazia
    {
        lista->inicio = no;
        lista->fim = no;
        no->proximo = NULL;
        no->anterior = NULL;
        lista->tamanho++;
        return TRUE;
    }

};

//Sempre iremos remover os dois primeiros elementos da lista ordenada, pois sempre serão os dois menores elementos.
void removerListaHuffman(LISTA_DUPLAMENTE_ENCADEADA_ORDENADA *lista)
{
    //Excluir o inicio e o inicio->proximo voltado ao algoritmo de huffman.

    NO* aux = (NO*) malloc(sizeof(NO));
    aux = lista->inicio->proximo; //guardo o proximo do inicio da lista

    lista->inicio->proximo = NULL; //faço o proximo do inicio apontar pra NULL

    aux->anterior = NULL; // faço o anterior do inicio->proximo apontar pra NULL (assim excluímos o inicio da lista)

    lista->inicio = aux->proximo; // faço o novo inicio da lista ser o aux->proximo (inicio->proximo == aux)

    aux->proximo = NULL; //faço o proximo do inicio->proximo apontar para NULL (assim excluímos o inicio->proximo da lista)
	if(lista->inicio != NULL)
	    lista->inicio->anterior = NULL; //O anterior do aux->proximo era o aux; aux será removido, então o novo anterior será NULL.

    lista->tamanho -= 2; //Retiramos os dois primeiros elementos da lista.
};

/*================================================ARVORE HUFFMAN===================================================*/

ARVORE_BINARIA *criar_arvore(NO* raiz) {

	ARVORE_BINARIA *arv = (ARVORE_BINARIA *)malloc(sizeof(ARVORE_BINARIA));
	if (arv != NULL) {
		arv->raiz = raiz;
	}
	return arv;
};

int ehNoFolha(NO* no)
{
    if(no->filhoesquerdo == NULL && no->filhodireito == NULL)
    {
        return TRUE;
    }
    else
        return FALSE;
};

void apagar_arvore_aux(NO *raiz) {
	if (raiz != NULL) {
		apagar_arvore_aux(raiz->filhoesquerdo);
		apagar_arvore_aux(raiz->filhodireito);
		free(raiz->anterior);
		free(raiz->proximo);
		free(raiz);
	}
};

void apagar_arvore(ARVORE_BINARIA *arvore) {
	apagar_arvore_aux(arvore->raiz);
	free(arvore);
	arvore = NULL;
};

//Soma a frequencia de dois nós.
int somarFrequencia(NO* no_inicio, NO* no_inicio_proximo)
{
    int novaFrequencia;

    novaFrequencia = no_inicio->frequencia + no_inicio_proximo->frequencia;

    return novaFrequencia;
};

/*Calcular o tamanho da sequencia máxima de bits na árvore(altura da árvore).*/
int alturaArvoreBinaria(NO *raiz)
{
    int esquerda, direita;

    if(raiz == NULL)
        return -1;

    esquerda = alturaArvoreBinaria(raiz->filhoesquerdo);
    direita = alturaArvoreBinaria(raiz->filhodireito);

    return ((esquerda > direita)? esquerda : direita) + 1;
};

int conversorParaBinario(int numero)
{
    //Se for igual a 1, quer dizer que chegamos ao final da conversão para binário.
    if(numero != 1)
        return (numero % 2); //Retorna zero ou um;
    else
        return 1;
};

//Escrever os bits mais a direita (começa a escrever nos bits menos significativos)
unsigned char escreverBit(int bit,unsigned char buffer)
{
    buffer <<= 1;

    if(bit)
        buffer |= 1;

    return buffer;
};
//Escrever os bits mais a esquerda (começa a escrever nos bits mais significativos)
unsigned char escreverBitInvertido(int bit, unsigned char buffer)
{
    buffer >>= 1;

    if(bit)
        buffer |= 128; //128 == 2^7 == 10000000 na base 2

    return buffer;
};

void posordem_sequencia_bitsValidos(NO* raiz,int contadorSequenciaBits,short int *sequencia, int caracteresPresentes, NO **nos_folhas,int k, int *qtdAdcionadosNoVetor)
{
	 if(!ehNoFolha(raiz))
    {
        //Indo para a esquerda na árvore, a sequencia recebe 0.
        sequencia[contadorSequenciaBits] = 0;
        posordem_sequencia_bitsValidos(raiz->filhoesquerdo,contadorSequenciaBits+1,sequencia,caracteresPresentes,nos_folhas,k, qtdAdcionadosNoVetor);
        //Indo para a esquerda na árvore, a sequencia recebe 1.
        sequencia[contadorSequenciaBits] = 1;
        posordem_sequencia_bitsValidos(raiz->filhodireito,contadorSequenciaBits+1,sequencia,caracteresPresentes,nos_folhas,k, qtdAdcionadosNoVetor);
    }
    else
    {
        //Buffer de sequencia auxialiar que irá receber a sequencia correspondente ao caracter do nó folha.
		unsigned char *aux_sequencia = (unsigned char*) malloc(sizeof(unsigned char)*k);
		int i = 0;

		unsigned char buffer_aux = 0;
		int bits_escritos_bitsValidos = 0, numero_aux = contadorSequenciaBits, bit_aux;

        //Escrever o numero de bits validos da sequencia do nó folha correspondente.
        //Para transformar um numero para binario, precisamos dividí-lo por 2 até que o resultado chegue a 1.
        //Mas, depois de terminando, precisamos ler os restos das divisões da direita para esquerda e de baixo pra cima.
        //Por isso foi criada a funcao "escreverBitInvertido", que começa a escrever o primeiro resto da divisão
        //(que será o bit mais significativo), no bit mais significativo e a lógica segue para todos os outros.
        while(numero_aux != 0) //quando numero_aux == 1, 1 / 2 == 0.5 ~= 0 (pois numero_aux é int).
        {
            bit_aux = conversorParaBinario(numero_aux);
            buffer_aux = escreverBitInvertido(bit_aux,buffer_aux);
            bits_escritos_bitsValidos++;
            numero_aux /= 2;
        }

        buffer_aux >>= (TAM_BUFFER - bits_escritos_bitsValidos);

        raiz->bitsValidos = buffer_aux;

        //Flush do buffer.
		for(i=0;i<k;i++)
			aux_sequencia[i] = 0;

        //Escreve a sequencia no buffer auxiliar.
		for(i=0;i<k*TAM_BUFFER;i++)
		{
            aux_sequencia[i/TAM_BUFFER] <<= 1;

			if(i<contadorSequenciaBits)
			{
				if(sequencia[i])
					aux_sequencia[i/TAM_BUFFER] |= 1;
			}
		}

        //Coloca esta sequencia no nó folha.
		raiz->sequenciaBits = aux_sequencia;
		nos_folhas[(*qtdAdcionadosNoVetor)] = raiz;
		(*qtdAdcionadosNoVetor)++;
    }
};

/*=================================CODIFICACAO E DECODIFICACÃO==================================*/

void codificar(NO **nos_folhas,int caracteresPresentes,FILE *arquivoOriginal,FILE *arquivoCompactado)
{
    int i = 0,j = 0,w = 0;
    unsigned char buffer_aux = 0;
    int bits_escritos_buffer_aux = 0;
    int iteracaoEscritaNoBuffer = 7;
    unsigned char caracterLido;
    int bitsValidos, bit_aux, bits_restantes_fim_texto;

    while(!feof(arquivoOriginal))
    {
        fread(&caracterLido,sizeof(unsigned char),1,arquivoOriginal);

        for(i = 0;i < caracteresPresentes;i++)
        {
            /*Se o caracter lido é igual ao caracter do nó folha, escrevemos sua sequencia no arquivo.*/
            if(nos_folhas[i]->caracter == caracterLido)
            {
                bitsValidos = (int)nos_folhas[i]->bitsValidos; //Para bits válidos sendo um unsigned char.

                /*Esse j realmente é para não ser usado dentro da iteração, só parar fazer o controle de quantos bits
                precisamos escrever.*/
                for (j = 1; j <= bitsValidos; j++,iteracaoEscritaNoBuffer--,bits_escritos_buffer_aux++)
                {
                    if(bits_escritos_buffer_aux == TAM_BUFFER)
                    {
                        fwrite(&buffer_aux,sizeof(unsigned char),1,arquivoCompactado); //escrever o buffer cheio no arquivo.
                        buffer_aux = 0; //flush no buffer.
                        bits_escritos_buffer_aux = 0; //zera o contador.
                    }

                    /*Então lemos todos os bits de um dos buffers da sequencia de buffers correspontentes.
                    Precisamos então ir para o próximo buffer e lê-lo desde o início (bit mais significativo).*/
                    if(iteracaoEscritaNoBuffer == -1)
                    {
                        w++; //próximo buffer.
                        iteracaoEscritaNoBuffer = 7; //começará a lê-lo do início.
                    }

                    bit_aux = (nos_folhas[i]->sequenciaBits[w] >> iteracaoEscritaNoBuffer) & 1;
                    buffer_aux = escreverBit(bit_aux,buffer_aux);
                }

                w = 0;
                iteracaoEscritaNoBuffer = 7;
                break;
            }
        }
    }

    /*Se sobrou alguns bits pertencentes a sequencia codificada, devemos escrevê-los.
    Isso é mostrado se a variável "bits_escritos_buffer_aux" for diferente de zero.*/
    if(bits_escritos_buffer_aux != 0) //Sobrou alguns bits para serem escritos.
    {
        /*Calcular quantas vezes precisamos deslocar o buffer
        (Lembre-se: os bits válidos de sequencia sempre ficaram mais a esquerda do buffer, e quando escrevemos
        os bits através da função: "escreverBit()", nós escrevemos sempre da direita para a esquerda.)*/
        bits_restantes_fim_texto = TAM_BUFFER - bits_escritos_buffer_aux;
        /*Realizando o deslocamento.*/
        buffer_aux <<= bits_restantes_fim_texto;
        /*Escrevendo o resultado no arquivo.*/
        fwrite(&buffer_aux,sizeof(unsigned char),1,arquivoCompactado);
    }
};

void decodificar(ARVORE_BINARIA *arvoreDeHuffman,int totalCaracteres,FILE *arquivoCompactado,FILE *arquivoDescompactado)
{
    NO *aux;
    int bit_aux, i = 0;
    unsigned char buffer_aux;
    unsigned char auxc;

    /*A ideia da decodificação é pecorrer a árvore a medida que se lê um bit na sequência compactada. Se lê o bit 1, vai pra direita,
    se lê o bit 0, vai para esquerda. Quando chegar um nó folha quer dizer que lemos um sequencia que corresponde a um caracter, então
    deve-se escrever este caracter no arquivo descompactado. O algoritmo para quando o contador "totalCaracteres" chegar a 0, o que 
    mostra que não temos mais caracteres para reconhecer e podemos para a nossa leitura de decodificação.*/

    aux = arvoreDeHuffman->raiz;

    while(totalCaracteres != 0)
    {
        fread(&buffer_aux,sizeof(unsigned char),1,arquivoCompactado);

        /*O índice do buffer com 8 bits é considerada da direita para esquerda.
        Ou seja: 00000110 -----> exemplo de buffer
                 76543210 -----> índices dos bits.*/
        for(i = 7;i >= 0;i--)
        {
            bit_aux = (buffer_aux >> i) & 1; //estratégia para ler o bit do buffer de índice i.

            if(bit_aux)
            {
                aux = aux->filhodireito;

                if(ehNoFolha(aux))
                {
                    auxc = aux->caracter;
                    fwrite(&auxc,sizeof(unsigned char),1,arquivoDescompactado);
                    totalCaracteres--;
                    aux = arvoreDeHuffman->raiz;

                    if(totalCaracteres == 0)
                        break;
                }
            }
            else
            {
                aux = aux->filhoesquerdo;

                if(ehNoFolha(aux))
                {
                    auxc = aux->caracter;
                    fwrite(&auxc,sizeof(unsigned char),1,arquivoDescompactado);
                    totalCaracteres--;
                    aux = arvoreDeHuffman->raiz;

                    if(totalCaracteres == 0)
                        break;
                }
            }
        }

        buffer_aux = 0;
    }
};

void construirHuffman(ARVORE_BINARIA *arvoreDeHuffman, TABELA *tabela, int caracteresPresentes)
{
    NO *aux;
    int i = 0, bitsValidos_aux,j = 0, bit_aux,w = 0;

    /*A ideia é reconstruir a árvore original da codificação para ser possível a decodificação. Aqui, para cada caracter,
    irá se realizar uma leitura bit a bit da sequencia do caracter e, de acordo com o valor, irá criar(ou andar caso o nó esteja criado)
    um nó a esquerda(caso o bit seja 0) ou a direita(caso o bit seja 1). Paramos quando não houvermos mais caracteres a serem postos
    na árvore.*/

    for(i = 0;i < caracteresPresentes;i++)
    {
        aux = arvoreDeHuffman->raiz;
        bitsValidos_aux = (int)tabela[i].bitsValidos;

        for(j = 0,w = 7;j < bitsValidos_aux;j++,w--)
        {
            if(w == -1)
                w = 7;

            bit_aux = (tabela[i].sequencia[j/TAM_BUFFER] >> w) & 1;

            if(bit_aux)
            {
                if(aux->filhodireito != NULL)
                {
                    aux = aux->filhodireito;
                }
                else
                {
                    aux->filhodireito = criar_no(0,0,0);
                    aux = aux->filhodireito;
                }
            }
            else
            {
                if(aux->filhoesquerdo != NULL)
                {
                    aux = aux->filhoesquerdo;
                }
                else
                {
                    aux->filhoesquerdo = criar_no(0,0,0);
                    aux = aux->filhoesquerdo;
                }
            }
        }

        //Saindo do for, o aux estará apontando pro nó folha. Portanto:
        aux->caracter = tabela[i].caracter;
    }
};