/*Arquivo header para simular uma biblioteca das funcoes usadas no programa.*/

#include "funcoesCodificacaoDeHuffman.c"

/*===================================LISTA DUPLAMENTE ENCADEADA ORDENADA================================*/
LISTA_DUPLAMENTE_ENCADEADA_ORDENADA *criar_lista();
int listaVazia(LISTA_DUPLAMENTE_ENCADEADA_ORDENADA *lista);
NO* criar_no(unsigned char caracter,int frequencia,int criar_no_caracter);
int inserirListaOrdenada(LISTA_DUPLAMENTE_ENCADEADA_ORDENADA *lista, NO* no);
void removerListaHuffman(LISTA_DUPLAMENTE_ENCADEADA_ORDENADA *lista);

/*================================================ARVORE HUFFMAN===================================================*/
ARVORE_BINARIA *criar_arvore(NO* raiz);
int ehNoFolha(NO* no);
void apagar_arvore_aux(NO *raiz);
void apagar_arvore(ARVORE_BINARIA *arvore);
int somarFrequencia(NO* no_inicio, NO* no_inicio_proximo);
int alturaArvoreBinaria(NO *raiz);
int conversorParaBinario(int numero);
void posordem_sequencia_bitsValidos(NO* raiz,int contadorSequenciaBits,short int *sequencia, int caracteresPresentes, NO **nos_folhas,int k, int *qtdAdcionadosNoVetor);

/*================================================CODIFICAÇÃO E DECODIFICAÇÃO===================================================*/
unsigned char escreverBit(int bit,unsigned char buffer);
unsigned char escreverBitInvertido(int bit, unsigned char buffer);
void codificar(NO **nos_folhas,int caracteresPresentes,FILE *arquivoOriginal,FILE *arquivoCompactado);
void decodificar(ARVORE_BINARIA *arvoreDeHuffman,int totalCaracteres,FILE *arquivoCompactado,FILE *arquivoDescompactado);
void construirHuffman(ARVORE_BINARIA *arvoreDeHuffman, TABELA *tabela, int caracteresPresentes);
