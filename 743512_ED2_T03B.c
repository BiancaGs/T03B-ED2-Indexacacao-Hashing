/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 03A - Hashing com encadeamento
 *
 * RA: 743512
 * Aluno: Bianca Gomes Rodrigues
 * ========================================================================== */

/* Bibliotecas */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 51
#define TAM_MARCA 51
#define TAM_DATA 11
#define TAM_ANO 3
#define TAM_PRECO 8
#define TAM_DESCONTO 4
#define TAM_CATEGORIA 51

#define TAM_REGISTRO 192
#define MAX_REGISTROS 1000
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)

/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n\n"
#define ARQUIVO_VAZIO "Arquivo vazio!"
#define INICIO_BUSCA 							 "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM						  "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO 						"********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO					    "********************************ARQUIVO*******************************\n"
#define INICIO_EXCLUSAO  "**********************EXCLUIR*********************\n"
#define SUCESSO  				 "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 "FALHA AO REALIZAR OPERACAO!\n"
#define REGISTRO_INSERIDO "Registro %s inserido com sucesso.\n\n"



/* Registro do Produto */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char marca[TAM_MARCA];
	char data[TAM_DATA];	/* DD/MM/AAAA */
	char ano[TAM_ANO];
	char preco[TAM_PRECO];
	char desconto[TAM_DESCONTO];
	char categoria[TAM_CATEGORIA];
} Produto;

/* Registro da Tabela Hash
 * Contém a chave primária, o RRN do registro atual e o ponteiro para o próximo
 * registro. */
typedef struct chave {
	char pk[TAM_PRIMARY_KEY];
	int rrn;
	struct chave *prox;
} Chave;

/* Estrutura da Tabela Hash */
typedef struct {
  int tam;
  Chave **v;
} Hashtable;

/* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
int nregistros;

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo();

/* Exibe o Produto */
int exibir_registro(int rrn);

/*Função de Hash*/
short hash(const char* chave, int tam);

/*Auxiliar para a função de hash*/
short f(char x);

/*Retorna o primeiro número primo >= a*/
int  prox_primo(int a);

/*Funções do Menu*/
void carregar_tabela(Hashtable* tabela);
void cadastrar(Hashtable* tabela);
int  alterar(Hashtable *tabela);
void buscar(Hashtable tabela);
int  remover(Hashtable* tabela);
void liberar_tabela(Hashtable* tabela);

/* <<< DECLARE AQUI OS PROTOTIPOS >>> */
void imprimir_tabela(Hashtable tabela);

Produto Recuperar_Registro(int RRN);

void gerarChave(Produto * Novo);

/*Cria e inicializa a TABELA com o TAMANHO inserido pelo usuário*/
void Criar_Tabela(Hashtable *Tabela, int Tamanho);

int Busca(char pk[], Hashtable *Tabela);

/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */

int main() {
	
	/* Arquivo */
	int carregarArquivo = 0;
	scanf("%d%*c", &carregarArquivo); // 1 (sim) | 0 (nao)
	if (carregarArquivo) 
		carregar_arquivo();

	/* Tabela Hash */
	int tam;
	scanf("%d%*c", &tam);
	tam = prox_primo(tam);

	Hashtable tabela;
	Criar_Tabela(&tabela, tam);
	if (carregarArquivo == 1) 
		carregar_tabela(&tabela);
	
	/* Execução do programa */
	int opcao = 0;
	while(opcao != 6) {
		scanf("%d%*c", &opcao);
		switch(opcao) {

		case 1:
			cadastrar(&tabela);
			break;
		case 2:
			printf(INICIO_ALTERACAO);
			if(alterar(&tabela))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 3:
			printf(INICIO_BUSCA);
			buscar(tabela);
			break;
		case 4:
			printf(INICIO_EXCLUSAO);
			printf("%s", (remover(&tabela)) ? SUCESSO : FALHA );
			break;
		case 5:
			printf(INICIO_LISTAGEM);
			imprimir_tabela(tabela);
			break;
		case 6:
			// liberar_tabela(&tabela);
			break;

		case 10:
			printf(INICIO_ARQUIVO);
			printf("%s\n", (*ARQUIVO!='\0') ? ARQUIVO : ARQUIVO_VAZIO);
			break;

		default:
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return 0;
}


/* <<< IMPLEMENTE AQUI AS FUNCOES >>> */

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo() {
	scanf("%[^\n]\n", ARQUIVO);
	nregistros = strlen(ARQUIVO)/TAM_REGISTRO;
}

/*Auxiliar para a função de hash*/
short f(char x){
	return (x < 59) ? x - 48 : x - 54; 
}

/* ----------------------------------------*/

/*Função de Hash*/
short hash(const char* chave, int tam){
	
	//Função Hash
	int h = 0;
	for(int i= 1; i <= 8; i++)
		h+= i*(f(chave[i-1]));

	// printf("H %d - Tamanho %d\n", h, tam);
	// printf("H%Tamanho %d\n", h%tam);
	return (h%tam);
}

int  prox_primo(int a){

	int Contador = 0;
	for(int i=1; i <= a; i++){
		if(a%i == 0)
			Contador++;
	}
	if(Contador == 2)
		return a;

	else{
		int flag = 0;
		while(flag == 0){			
			Contador = 0;
			a++;
			
			for(int j=1; j <= a; j++){
				if(a%j == 0)
					Contador++;
			}
			if(Contador == 2){
				flag = 1;
				return a;
			}	
		}				
	}
}

/*Cria e inicializa a TABELA com o TAMANHO inserido pelo usuário*/
void Criar_Tabela(Hashtable *Tabela, int Tamanho){
	
    Tabela->tam = Tamanho;
	Tabela->v = (Chave**)malloc(Tamanho*sizeof(Chave));
	
}
void carregar_tabela(Hashtable* tabela){

	// printf("nRegistros %d\n", nregistros);
	
	for(int i =0; i<nregistros; i++){
		
		int Posicao = hash(Recuperar_Registro(i).pk, tabela->tam);

		if(tabela->v[Posicao] == NULL){

			// printf("tabela->v[Posicao] == NULL\n");
			// printf("Posicao %d\n", Posicao );

			Chave *newNode = (Chave*)malloc(sizeof(Chave));
			
			strcpy(newNode->pk, Recuperar_Registro(i).pk);
			newNode->rrn = i;
			newNode->prox = NULL;

			tabela->v[Posicao] = newNode;

			// printf(REGISTRO_INSERIDO, Novo.pk);
			// return;
		}
		else{
				// printf("Posicao %d\n", Posicao );

				int flag = 0;

				if(flag == 0 && strcmp(tabela->v[Posicao]->pk, Recuperar_Registro(i).pk) > 0){
					Chave * New = (Chave*)malloc(sizeof(Chave));
					strcpy(New->pk,Recuperar_Registro(i).pk);
					New->rrn = i;
					New->prox = tabela->v[Posicao];
					tabela->v[Posicao] = New;
					flag = 1;
				}

				//else - Não funcionava sempre com o else				
				/*Monitoria*/
				Chave * Anterior = NULL; 
				Chave * Atual = tabela->v[Posicao];

				while (Atual->prox!= NULL && strcmp(Recuperar_Registro(i).pk, Atual->pk) > 0){
					Anterior = Atual;
					Atual = Atual->prox;
				}

				if(flag == 0 && Anterior != NULL && strcmp(Atual->pk, Recuperar_Registro(i).pk) > 0){
					Chave * New = (Chave*)malloc(sizeof(Chave));	
					strcpy(New->pk,Recuperar_Registro(i).pk);
					New->rrn = i;
					New->prox = Atual;
					Anterior->prox = New;
					flag = 1;
				}
				
				if(flag == 0 && strcmp(Atual->pk, Recuperar_Registro(i).pk) < 0){
					Chave * New = (Chave*)malloc(sizeof(Chave));
					strcpy(New->pk,Recuperar_Registro(i).pk);
					New->rrn = i;
					Atual->prox = New;
					New->prox = NULL;
					flag = 1;
				}

				// if(flag == 1)
				// 	printf(REGISTRO_INSERIDO, Novo.pk);
		}
	}
}

Produto Recuperar_Registro(int RRN){
	
	char Registro[193], *p;
	strncpy(Registro, ARQUIVO + ((RRN)*192), 192);
	Registro[192] = '\0';

	Produto A;
	p = strtok(Registro,"@");
	strcpy(A.pk,p);
	p = strtok(NULL,"@");
	strcpy(A.nome,p);
	p = strtok(NULL,"@");
	strcpy(A.marca,p);
	p = strtok(NULL,"@");
	strcpy(A.data,p);
	p = strtok(NULL,"@");
	strcpy(A.ano,p);
	p = strtok(NULL,"@");
	strcpy(A.preco,p);
	p = strtok(NULL,"@");
	strcpy(A.desconto,p);
	p = strtok(NULL,"@");
	strcpy(A.categoria,p);

	return A;
}

/* Exibe o Produto */
int exibir_registro(int rrn){
	if(rrn<0)
		return 0;
	float preco;
	int desconto;
	Produto j = Recuperar_Registro(rrn);
  char *cat, categorias[TAM_CATEGORIA];
	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.marca);
	printf("%s\n", j.data);
	printf("%s\n", j.ano);
	sscanf(j.desconto,"%d",&desconto);
	sscanf(j.preco,"%f",&preco);
	preco = preco *  (100-desconto);
	preco = ((int) preco)/ (float) 100 ;
	printf("%07.2f\n",  preco);
	//strncpy(categorias, j.categoria, strlen(j.categoria));
    // for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
    // printf("%s ", cat);

	strcpy(categorias, j.categoria);

	cat = strtok (categorias, "|");

	while(cat != NULL){
		printf("%s", cat);
		cat = strtok (NULL, "|");
		if(cat != NULL)
			printf(" ");
	}
	printf("\n");
	
    return 1;
}

void gerarChave(Produto * Novo){

	Novo->pk[0] = Novo->nome[0];
	Novo->pk[1] = Novo->nome[1];
	Novo->pk[2] = Novo->marca[0];
	Novo->pk[3] = Novo->marca[1];
	Novo->pk[4] = Novo->data[0];
	Novo->pk[5] = Novo->data[1];
	Novo->pk[6] = Novo->data[3];
	Novo->pk[7] = Novo->data[4];
	Novo->pk[8] = Novo->ano[0];
	Novo->pk[9] = Novo->ano[1]; 
	Novo->pk[10] = '\0';

}

void cadastrar(Hashtable* tabela){
	//Código - NÃO é inserido pelo usuário 
	// char pk[TAM_PRIMARY_KEY];
	// gerarChave(novo);

	/*-----------------------*/

	/* Interação com o Usuário */
	Produto Novo;
	/* CAMPOS DE TAMANHO VARIÁVEL */
	
	//Nome do Produto ou Modelo
	// char Nome[TAM_NOME];
	scanf("%[^\n]s", Novo.nome);
	getchar();
	//Marca
	// char Marca[TAM_MARCA];
	scanf("%[^\n]s", Novo.marca);
	getchar();
	/*-----------------------*/

	/* CAMPOS DE TAMANHO FIXO */

	//Data de Registro
	// char Data[TAM_DATA];	/* DD/MM/AAAA */
	scanf("%[^\n]s", Novo.data);
	getchar();
	//Ano de Lançamento
	// char Ano[TAM_ANO];
	scanf("%[^\n]s", Novo.ano);
	getchar();
	//Preço-Base
	// char Preço[TAM_PRECO];
	scanf("%[^\n]s", Novo.preco);
	getchar();
	//Desconto
	// char Desconto[TAM_DESCONTO];
	scanf("%[^\n]s", Novo.desconto);
	getchar();
	//Categorias
	// char Categoria[TAM_CATEGORIA];
	scanf("%[^\n]s", Novo.categoria);
	getchar();
	/*-----------------------*/

	gerarChave(&Novo);

	//Verifica se o PRODUTO existe
	if(Busca(Novo.pk, tabela) > -1) {
		printf(ERRO_PK_REPETIDA, Novo.pk);
		return;
 	}

	// else{
		
		nregistros++;

		//Registro Auxiliar
		char rAuxiliar[193]; //TAM_REGISTRO
		rAuxiliar[192] = '\0';

		sprintf(rAuxiliar, "%s@%s@%s@%s@%s@%s@%s@%s@", Novo.pk, Novo.nome, Novo.marca, Novo.data, Novo.ano, Novo.preco, Novo.desconto, Novo.categoria);

		//Precisamos obter o TAMANHO do REGISTRO AUXILIAR (rAuxiliar) para sabermos quantos "bytes" faltam para preencher totalmento o REGISTRO.
		int Tamanho = strlen(rAuxiliar);

		/*COMENTAR*/
		//printf("\nTamanho = %d", Tamanho);

		int i;
		//Preenchendo o REGISTRO por completo (192bytes)
		for(i = Tamanho; i < 192; i++)
			rAuxiliar[i] = '#';

		/*COMENTAR*/
		//printf("\nTamanho - Final = %d", strlen(rAuxiliar));
		//printf("\nRegistro: %s\n\n", rAuxiliar);

		strcat(ARQUIVO, rAuxiliar);

		//int tRegistro = strlen(rAuxiliar);
		//printf("%d\n", tRegistro);
		
		//printf("%s\n", rAuxiliar);
		
		//printf("%s\n", ARQUIVO);
		int Posicao = hash(Novo.pk, tabela->tam);

		if(tabela->v[Posicao] == NULL){

			// printf("tabela->v[Posicao] == NULL\n");
			// printf("Posicao %d\n", Posicao );

			Chave *newNode = (Chave*)malloc(sizeof(Chave));
			
			strcpy(newNode->pk, Novo.pk);
			newNode->rrn = nregistros-1;
			newNode->prox = NULL;

			tabela->v[Posicao] = newNode;

			printf(REGISTRO_INSERIDO, Novo.pk);
			return;
		}
		else{
				// printf("Posicao %d\n", Posicao );

				int flag = 0;

				if(flag == 0 && strcmp(tabela->v[Posicao]->pk, Novo.pk) > 0){
					Chave * New = (Chave*)malloc(sizeof(Chave));
					strcpy(New->pk,Novo.pk);
					New->rrn = nregistros-1;
					New->prox = tabela->v[Posicao];
					tabela->v[Posicao] = New;
					flag = 1;
				}

				//else - Não funcionava sempre com o else				
				/*Monitoria*/
				Chave * Anterior = NULL; 
				Chave * Atual = tabela->v[Posicao];

				while (Atual->prox!= NULL && strcmp(Novo.pk, Atual->pk) > 0){
					Anterior = Atual;
					Atual = Atual->prox;
				}

				if(flag == 0 && Anterior != NULL && strcmp(Atual->pk, Novo.pk) > 0){
					Chave * New = (Chave*)malloc(sizeof(Chave));	
					strcpy(New->pk,Novo.pk);
					New->rrn = nregistros-1;
					New->prox = Atual;
					Anterior->prox = New;
					flag = 1;
				}
				
				if(flag == 0 && strcmp(Atual->pk, Novo.pk) < 0){
					Chave * New = (Chave*)malloc(sizeof(Chave));
					strcpy(New->pk,Novo.pk);
					New->rrn = nregistros-1;
					Atual->prox = New;
					New->prox = NULL;
					flag = 1;
				}

				if(flag == 1)
					printf(REGISTRO_INSERIDO, Novo.pk);
		}
}


int alterar(Hashtable *tabela){

	char PK[TAM_PRIMARY_KEY];
	memset(PK, '\0', TAM_PRIMARY_KEY);

	scanf("%[^\n]s", PK);
	getchar();

	if(Busca(PK, tabela) == -1){
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}

	else{

		int RRN = Busca(PK, tabela);
		//printf("%d\n", RRN);
		
		//O desconto inserido precisa ser de 3 bytes com valor entre 000 e 100.
		char Desconto[4];
		memset(Desconto, '\0', 4);

		int flag = 0;				
		//getchar();
		scanf("%[^\n]s", Desconto);
		getchar();

		if(strcmp(Desconto, "000") >= 0 && strcmp(Desconto, "100") <= 0){ //&& strlen(Desconto) == 3){
				flag = 1;
		}
		while(flag == 0){	
			printf(CAMPO_INVALIDO);
			//getchar();	
			scanf("%[^\n]s", Desconto);
			getchar();
			
			if(strcmp(Desconto, "000") >= 0 && strcmp(Desconto, "100") <= 0){ //&& strlen(Desconto) == 3){
					flag = 1;
			}
		}

		char *Arquivo = ARQUIVO + RRN * 192;
		// printf("%s\n", Arquivo);

		int Contador = 0;
		while(Contador < 6){
			if((*Arquivo) == '@')
				Contador++;
			Arquivo++;
		}
		//printf("Contador %d\n", Contador);
		// printf("%s\n", Arquivo);

		
		for(int i = 0; i < 3; i++){
			// printf("%c ", *Arquivo);
			*Arquivo = Desconto[i];
			Arquivo++;
		}
		return 1;
	}
}

int Busca(char pk[], Hashtable *tabela){

	int Posicao = hash(pk, tabela->tam);

	Chave *Node = tabela->v[Posicao];

	while(Node != NULL){
		if(strcmp(Node->pk, pk)==0){
		//	exibir_registro(tabela->v[Posicao]->pk);
			return Node->rrn;
		}
		Node = Node->prox;
	}
	// printf(REGISTRO_N_ENCONTRADO);
	return -1;

}

void buscar(Hashtable tabela){

	char pk[TAM_PRIMARY_KEY];
	memset(pk, '\0', TAM_PRIMARY_KEY);

	scanf("%[^\n]s", pk);
	getchar();

	int Posicao = hash(pk, tabela.tam);

	Chave *Node = tabela.v[Posicao];

	while(Node != NULL){
		if(strcmp(Node->pk, pk)==0){
			exibir_registro(Node->rrn);
			return;
		}
		Node = Node->prox;
	}
	printf(REGISTRO_N_ENCONTRADO);

}

int remover(Hashtable* tabela){

	char pk[TAM_PRIMARY_KEY];
	memset(pk, '\0', TAM_PRIMARY_KEY);

	scanf("%[^\n]s", pk);
	getchar();


	if(Busca(pk, tabela) == -1){
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}

	int Posicao = hash(pk, tabela->tam);

	Chave *Anterior = NULL;
	Chave *Node = tabela->v[Posicao];

	if(strcmp(Node->pk, pk)==0){
		//Primeiro, mas não ÚNICO, elemento da lista
		if(Node->prox != NULL){
			tabela->v[Posicao] = tabela->v[Posicao]->prox;
			free(Node);
			return 1;
		}
		else{
			// Primeiro e ÚNICO elemento da lista
			tabela->v[Posicao] = NULL;
			free(Node);
			return 1;
		}
	}

	Anterior = Node;
	Node = Node->prox;

	while(Node != NULL){
		if(strcmp(Node->pk, pk)==0){
			Anterior->prox = Node->prox;
			return 1;
		}
		Anterior = Node;
		Node = Node->prox;
	}
	printf(REGISTRO_N_ENCONTRADO);
	return 0;

}


void liberar_tabela(Hashtable* tabela){

	Chave *Anterior;
	Chave *Node;

	for(int i=0; i<tabela->tam; i++){
	
		Node = tabela->v[i];

		while (Node) {
			Anterior = Node;
			free(Node);
			Node = Node->prox;
		}


	}

	tabela->tam = 0;
	free(tabela->v);



}

void imprimir_tabela(Hashtable tabela){

	for(int i=0; i<tabela.tam; i++){
	
		printf("[%d]", i);
		
		Chave *Node = tabela.v[i];

		while(Node != NULL){
			printf(" %s", Node->pk);
			Node = Node->prox;
		} 

		printf("\n");	
	}
}
