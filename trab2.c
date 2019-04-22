//Segundo trabalho da disciplina de ORD 2018
//ALUNO: GUSTAVO GRAVENA BATILANI RA88501
//ALUNO: LUIZ FLAVIO PEREIRA      RA91706

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEYS   4
#define NO         0
#define YES        1
#define NIL        (-1)
#define NOTHING    0
#define ZERO       0
#define UM         1

//estrutura da arvore
typedef struct{
	int keycount;
	int key[MAX_KEYS];
	int child[MAX_KEYS + UM];
} BTPAGE;

//menu do programa
int showMenu(){
	int opcao;
    
	printf("*-------------------------------------------------------- MENU --------------------------------------------------------*");
    printf("[1] Importar arquivo de chave(s).\n[2] Inserir chave.\n[3] Buscar Chave.\n[4] Listar Arvore-B.\n[0] Sair.\n\n");
    printf("Opcao: ");
	scanf("%d", &opcao);

    return opcao;
}

//funcao para obter o rrn
int newRRN(FILE *p_auxiliar){
    fseek(p_auxiliar, ZERO, SEEK_END); //move ponteiro para EOF
    int offSet = ftell(p_auxiliar), sizePage, sizeInt, rrn;
    
	sizePage = sizeof(BTPAGE);
    sizeInt  = sizeof(int);
    
    return rrn = ((offSet - sizeInt) / sizePage); //calcula o rrn corretamente (slide da profa);
}

//procedimento que escreve nova raiz no comeco do arquivo 'arvores.txt'
void putRoot(FILE *p_escrita, int root){
	fseek(p_escrita, ZERO, SEEK_SET);
    fwrite(&root, sizeof(root), UM, p_escrita); 
}

//procedimento responsavel por fazer a inicializacao de uma nova pagina
void initializePage(BTPAGE *NEWPAGE, int i){
	NEWPAGE->keycount = ZERO;
    for(i = ZERO; i < MAX_KEYS; i++)  NEWPAGE->key[i]   = ZERO;
    for(i = ZERO; i <= MAX_KEYS; i++) NEWPAGE->child[i] = NIL;
}

//funcao que verifica a existencia do arquivo 'arvores.txt'
int check_B_Tree(){
	FILE *p_leitura = fopen("arvores.txt","r");
    if(!p_leitura){ 
        printf("O arquivo 'chaves.txt' ainda nao foi importado!\n");
        return ZERO;
    }
    fclose(p_leitura);
    return UM;
}

//procedimento para inserir na pagina
void toInsertInThePage(int key, int filho_direito, BTPAGE *PAGE){
    int i;
    for(i = PAGE->keycount; (key < PAGE->key[i - UM]) && (i > ZERO); i--){
        PAGE->key[i] = PAGE->key[i - UM];
        PAGE->child[i + UM] = PAGE->child[i];
    }
    PAGE->keycount++;
    PAGE->key[i] = key;
    PAGE->child[i + UM] = filho_direito;
}

//procedimento responsavel por fazer a divisao em caso de OVERFLOW da pagina
void split(FILE* p_auxiliar, int chave_i, int rrn_i, BTPAGE *PAGE, int *chave_pro, int* filho_direito_pro, BTPAGE *NEWPAGE){
    int i, keys[MAX_KEYS + UM], child[MAX_KEYS + 2];

    //move as chaves e filhos da antiga pagina para estes novos vetores
    for(i = ZERO; i < MAX_KEYS; i++){ 
        keys[i] = PAGE->key[i];
        child[i] = PAGE->child[i];
    }
    child[i] = PAGE->child[i];
    
    //insere a nova chave
	for(i = MAX_KEYS; (chave_i < keys[i - UM]) && (i > ZERO); i--){ 
        keys[i] = keys[i - UM];
        child[i + UM] = child[i];
    }
    keys[i] = chave_i;
    child[i + UM] = rrn_i;

    //passando o novo valor do rrn da nova pagina
    *filho_direito_pro = newRRN(p_auxiliar); 

	//inicializando nova pagina
    initializePage(NEWPAGE, i);

    //basicamente, move a primeira metade das chaves a PAGE e a segunda metade para a NEWPAGE
    for(i = ZERO; i < (MAX_KEYS / 2); i++){ 
        PAGE->key[i] = keys[i];
        PAGE->child[i] = child[i];
        NEWPAGE->key[i] = keys[i + UM + (MAX_KEYS / 2)];
        NEWPAGE->child[i] = child[i + UM + (MAX_KEYS / 2)];
        PAGE->key[i + (MAX_KEYS / 2)] = ZERO; //marca a segunda metade da PAGE como se ela estivesse vazia(0 = ZERO)
        PAGE->child[i + UM + (MAX_KEYS / 2)] = NIL; //marca a segunda metade da PAGE como se ela nao tivesse filhos(-1 = NIL)
    }

    PAGE->child[(MAX_KEYS / 2)] = child[(MAX_KEYS / 2)];
    NEWPAGE->child[(MAX_KEYS / 2)] = child[i + UM + (MAX_KEYS / 2)];
    NEWPAGE->keycount = MAX_KEYS - (MAX_KEYS / 2);
    PAGE->keycount = (MAX_KEYS / 2);
    *chave_pro = keys[(MAX_KEYS / 2)]; //aqui realiza a promocao da chave mediana
}
	
int insert(FILE *p_auxiliar, int rrn_pag_atual, int key, int *filho_direito_pro, int *chave_pro){
    BTPAGE *PAGE = malloc(sizeof(BTPAGE)), *NEWPAGE = malloc(sizeof(BTPAGE)); //alocando espaco para as paginas
	int i, pos = ZERO, rrn_pro, key_pro, offSet;
    
    if(rrn_pag_atual == NIL){ 
        *chave_pro = key;
        *filho_direito_pro = NIL;
        return YES;
    }
	else{
        offSet = sizeof(int) + (rrn_pag_atual * sizeof(BTPAGE)); // obtendo o offSet
        fseek(p_auxiliar, offSet, SEEK_SET); 
        fread(PAGE, sizeof(BTPAGE), UM, p_auxiliar); 
        for(i = ZERO; i < PAGE->keycount; i++){
            if(key > PAGE->key[i]) pos++;
        }
    }
	
	if(key == PAGE->key[pos]){
    	system("cls");
        printf("\t\tE R R O\n\n\t   CHAVE DUPLICADA!!!\n");
        return NIL; 
    }
    
    //o erro estava no IF(chamava a funcao insert para comparar com NO e com NIL, isso fazia com que ela tivesse um comportamento incorreto)
    int validador = insert(p_auxiliar, PAGE->child[pos], key, &rrn_pro, &key_pro);
    if(validador == NO || validador == NIL){
        return validador;
    }else{
		if(PAGE->keycount < MAX_KEYS){
        	offSet = sizeof(int) + (rrn_pag_atual * sizeof(BTPAGE)); // obtendo o offSet
        	toInsertInThePage(key_pro, rrn_pro, PAGE);
	        fseek(p_auxiliar, offSet, SEEK_SET); //Posiciona o ponteiro no registro
	        fwrite(PAGE, sizeof(BTPAGE), UM, p_auxiliar);
	        return NO;
    	}else{
	        split(p_auxiliar, key_pro, rrn_pro, PAGE, chave_pro, filho_direito_pro, NEWPAGE);
	        offSet = sizeof(int) + (rrn_pag_atual * sizeof(BTPAGE)); // obtendo o offSet
	        fseek(p_auxiliar, offSet, SEEK_SET);
	        fwrite(PAGE, sizeof(BTPAGE), UM, p_auxiliar); //Escreve  PAG em rrn_pag_atual
	        offSet = sizeof(int) + ((*filho_direito_pro) * sizeof(BTPAGE)); // obtendo o offSet
	        fseek(p_auxiliar, offSet, SEEK_SET);
	        fwrite(NEWPAGE, sizeof(BTPAGE), UM, p_auxiliar); //Escreve  NOVAPAG em filho_direito_pro
        	return YES;
    	}	
	}
    fclose(p_auxiliar);
}

//funcao que cria a raiz e solicita gravacao da nova raiz no comeco do arquivo
int createRoot(FILE *p_auxiliar, int key, int left, int right){
    int root = newRRN(p_auxiliar), i = ZERO, offSet;
    
    //inicializando nova pagina
    BTPAGE *PAGE = malloc(sizeof(BTPAGE));
	initializePage(PAGE, i);

	//atribuindo os valores a pagina
    PAGE->key[ZERO] = key;
    PAGE->child[ZERO] = left;
    PAGE->child[UM] = right;
    PAGE->keycount = UM;
    
	offSet = sizeof(root) + (root * sizeof(BTPAGE)); //obtendo o offSet e posicionando o ponteiro corretamente
    fseek(p_auxiliar, offSet, SEEK_SET);
    
	fwrite(PAGE, sizeof(BTPAGE), UM, p_auxiliar); //escrevendo pagina no arquivo 'arvores.txt'
    
    putRoot(p_auxiliar, root); //solicitando gravacao de nova raiz
    
	return root;
}

//procedimento que importa chaves do arquivo de entrada
void import(){
	fflush(stdin); //limpando buffer do teclado
	int i, qtde_chaves, key, filho_direito_pro, chave_pro, root = ZERO;
	char nome_arquivo[20], *nome_arq;
	
	//obtendo nome do arquivo e fazendo tratativa para nao ser necessario digitar o '.txt'
	printf("Insira o NOME do arquivo de importacao: "); 
    fgets(nome_arquivo, 20, stdin);
    nome_arquivo[strlen(nome_arquivo) - UM] = '.';
    nome_arq = strtok(nome_arquivo, ".");
	strcat(nome_arquivo, ".txt");

	//abrindo arquivo de leitura e de escrita
	system("cls"); 
    FILE *p_leitura = fopen(nome_arq,"r");
    if(!p_leitura){
    	printf("O arquivo \"%s\" nao existe, tente importar novamente!\n\n \t\tPROGRAMA ABROTADO!", nome_arq);
    	exit(ZERO);
    }
    printf("Ola, estamos criando o arquivo 'arvores.txt', por favor, aguarde...\n");
    FILE *p_escrita = fopen("arvores.txt", "wb+"); 
	
	//gravando raiz no cabecalho
    putRoot(p_escrita, root);
	
	//inicializando nova pagina
	BTPAGE* PAGE = malloc(sizeof(BTPAGE));
    initializePage(PAGE, i);

    fwrite(PAGE, sizeof(BTPAGE), UM, p_escrita);
    fscanf(p_leitura, "%d", &qtde_chaves);

	//fazendo leitura e inserindo cada chave do arquivo de entrada
    for(i = ZERO; i < qtde_chaves; i++){
        fscanf(p_leitura, "%d", &key);
        if(insert(p_escrita, root, key, &filho_direito_pro, &chave_pro) == YES){
            root = createRoot(p_escrita, chave_pro, root, filho_direito_pro);
        }
    }
    printf("\nO arquivo 'arvores.txt' foi criado com exito!\n");
	fclose(p_leitura);
    fclose(p_escrita); 
}

//procedimento para imprimir a pagina
void printPage(int rrn, BTPAGE *Page_print){
	int i;
	printf("RRN...: | %d ", rrn);
    printf("|\nChaves: ");
    for(i = ZERO; i < MAX_KEYS ; i++) printf("| %.6d ", (*Page_print).key[i]);
    printf("|\nFilhos: ");
	for(i = ZERO; i < MAX_KEYS + UM; i++) printf ("| %d ", (*Page_print).child[i]);
    printf("|\n");
}

//procedimento responsavel por listar a arvore
void list_B_Tree(){
	int raiz, RRN, enf_of_file, b_offSet, i;
	BTPAGE *Page_print = malloc(sizeof(BTPAGE)); //alocando espaco na memoria
	
	//abrindo o arquivo 'arvores.txt'
	FILE* p_leitura= fopen("arvores.txt", "rb"); 
	if(!p_leitura)
        printf("\nE R R O\n\nArquivo nao foi aberto com sucesso!");
	fread(&raiz, sizeof(int), UM, p_leitura); //lendo cabecalho

	printf("------------------------------------------------------- LISTAGEM -------------------------------------------------------");
    enf_of_file = fgetc(p_leitura);
	fseek(p_leitura, 4, SEEK_SET);
	while(enf_of_file != EOF){
		//obtendo offset e RRN
		b_offSet = ftell(p_leitura); 
        RRN = (b_offSet - sizeof(int)) / sizeof(BTPAGE);
		
		//realizando a identificacao da pagina raiz
        if(RRN == raiz) {
        	printf("--------------------------------------------------------- RAIZ ---------------------------------------------------------");
		}

		//fazendo leitura de uma pagina e mandando imprimi-la
		fread(Page_print, sizeof(BTPAGE), UM, p_leitura); 
		printPage(RRN, Page_print);
		for(i = ZERO; i < 120; i++) printf("-");
		
		//para manutenção do while	
        enf_of_file = fgetc(p_leitura);
        fseek(p_leitura, - UM, SEEK_CUR);
    }
    fclose(p_leitura);   
}

//procedimento que busca chave(slide da profa)
void toFoundKey(int rrn, int keySearch, int *rrn_encontrado, int *pos_encontrada, FILE *p_leitura, int offSet, BTPAGE *PAGE_AUX, int i){ 
	int pos = ZERO;
	
	if(rrn == NIL){ 
        printf("Chave nao encontrada, tente novamente mais tarde!\n");
    }else{
        offSet = (rrn * sizeof(BTPAGE)) + sizeof(rrn);
		fseek(p_leitura, offSet, SEEK_SET);
        fread(PAGE_AUX, sizeof(BTPAGE), UM, p_leitura);
        for(i = ZERO; i < PAGE_AUX->keycount; i++)
            if(keySearch > PAGE_AUX->key[i]) pos++;
        
        if(keySearch == PAGE_AUX->key[pos]){
            *rrn_encontrado = rrn;
            *pos_encontrada = pos;
            printPage(rrn, PAGE_AUX);
        }else{
			toFoundKey(PAGE_AUX->child[pos], keySearch, rrn_encontrado, pos_encontrada, p_leitura, offSet, PAGE_AUX, i);
        }
    }
}

//procedimento que realiza a busca
void search(){
	int keySearch, rrn_encontrado, pos_encontrada, i, rrn, offSet; 
	BTPAGE* PAGE_AUX = malloc(sizeof(BTPAGE));
	
	//abrindo arquivo 
	FILE *p_leitura = fopen("arvores.txt", "rb");
    if(!p_leitura)
        printf("\n\t\tE R R O\n\nArquivo nao foi aberto com sucesso!");
    fread(&rrn, sizeof(rrn), UM, p_leitura);
	offSet = (rrn * sizeof(BTPAGE)) + sizeof(rrn); //obtendo byte-offset da raiz
	
    //obtendo chave a ser buscada
    printf("Insira a chave que deseja BUSCAR: ");
    scanf("%d", &keySearch);
    
    //procurar chave a ser buscada
    for(i = ZERO; i < 120; i++) printf("-");
	toFoundKey(rrn, keySearch, &rrn_encontrado, &pos_encontrada, p_leitura, offSet, PAGE_AUX, i);
    
    fclose(p_leitura);
}

//procedimento responsavel por inserir uma nova chave
void insertNewKey(){
	int newKey, filho_direito_pro, chave_pro, root;
	
	//abrindo arquivo
	FILE *p_escrita = fopen("arvores.txt", "rb+"); //abrindo o arquivo 'arvores.txt'
	if(!p_escrita)
        printf("\n\t\tE R R O\n\nArquivo nao foi aberto com sucesso!");
	fread(&root, sizeof(root), UM, p_escrita); //lendo rrn da raiz
	
	//obtendo nova chave
	printf("Insira a NOVA CHAVE: ");
	scanf("%d", &newKey);
	
	//condicao para tamanho correto da chave
	if((newKey > 999999) || newKey < -999999){
		printf("\nTamanho de chave nao tolerado, tente novamente!\n");
		return;
	}

	//inserindo nova chave
    if(insert(p_escrita, root, newKey, &filho_direito_pro, &chave_pro) == YES)
        root = createRoot(p_escrita, chave_pro, root, filho_direito_pro);
	fclose(p_escrita);	
}

//procedimento que faz insercao quando ainda nao ha arvore e nem foi importado
void insertNO_B_Tree(){
	int root = ZERO, i, newKey, filho_direito_pro, chave_pro;
                	
    //abrindo arquivo
	for(i = ZERO; i < 120; i++) printf("-");
	printf("Criamos o arquivo 'arvores.txt'.\n");
    FILE *p_escrita = fopen("arvores.txt", "wb+");
    putRoot(p_escrita, root); //gravando raiz no cabecalho
					
	//obtendo nova chave
	printf("Insira a NOVA CHAVE: ");
	scanf("%d", &newKey);
		
	//condicao para tamanho correto da chave
	if((newKey > 999999) || newKey < -999999){
		printf("\nTamanho de chave nao tolerado, tente novamente!\n");
		return;
	}
				
	//inicializando nova pagina
	BTPAGE *PAGE = malloc(sizeof(BTPAGE));
	initializePage(PAGE, i);
	fwrite(PAGE, sizeof(BTPAGE), UM, p_escrita);
	
	//inserindo nova chave
	if(insert(p_escrita, root, newKey, &filho_direito_pro, &chave_pro) == YES)
        root = createRoot(p_escrita, chave_pro, root, filho_direito_pro);				
	fclose(p_escrita);	
}

//programa principal
void main(void){
	int opcao = ZERO;
    
    opcao = showMenu(opcao);    
	system("cls");
    while(opcao != ZERO){
        switch(opcao){
            case 1 : {
                system("cls");
				import();
                break;
            }
            case 2 : {
                system("cls");
                if(check_B_Tree())
                    insertNewKey();
                else
                	insertNO_B_Tree();	    
                break;
            }
            case 3 : {
                system("cls");
                if(check_B_Tree())
                    search();
                break;
            }
            case 4 : {
                system("cls");
                if(check_B_Tree())
                	list_B_Tree();
                break;
            }
            default :{
            	system("cls");
				printf("Opcao invalida, tente novamente!\n");
				break;
			}
        }
        opcao = showMenu();
        system("cls");
    }
}
