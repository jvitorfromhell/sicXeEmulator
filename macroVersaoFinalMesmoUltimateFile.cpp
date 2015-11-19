#include <iostream>
#include <streambuf>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include "SIC.h"
#include "simulador.h"

using namespace std;
		
struct Node {
		char str[40] ;
		struct Node *next;
};		

typedef struct Node node;

struct Macro {
		char name[40];
		struct Node *argList; // USADO APENAS PELA TABELA DE ARGUMENTOS E DE DEFINIÇAO
		struct Node *defList;
		struct Macro *next;
};

typedef struct Macro macro;

macro *macroList = (macro*) malloc(sizeof(macro));

bool linhaNula (string linha);
node* inserirNode (node *no,string txt);
macro* inserirMacro (macro *ma,string name);
void imprime (node *no);
bool listaVazia (node *no);
int definir_macro (string linha,ifstream *arq);
node* definir_macro_aninhada (macro *ma, node *macroDef,node *listChamada);			
string substituiArgumentos (string linha,node *paramReais,node *paramVirtuais);
int isOpcode (string leitura);

int processadorMacro(string codigoEntrada, string codigoSaida){
	int teste=0;
	char c;
	macro *i;
	node *j;
	node *k;
	node *listChamada = (node*) malloc(sizeof(node));			// lista de parametros reais a serem substituidos
	string linha,linhaExpandida,palavra;
	listChamada->next = NULL;
	macroList->next = NULL;
	ifstream arq (codigoEntrada);
	ofstream saida ("codigoExpandido.txt");
	
	getline( arq, linha);
	while( !arq.eof()){											//
		//cout << linha << endl;								//
		if(linha.find( "MACRO") != linha.npos){					// Definições de macros
			teste = definir_macro(linha,&arq);	
			if(teste == 3){										//
				cout << "EROOOOOOOOOOOOOOO - Faltou um MEND" << endl;
				arq.close();
				saida.close();
				return 3;
			}
		}
		else{													//
			if(!linhaNula(linha))							//
				break;											//
			}													//
		getline(arq, linha);									//
	}					


											//

	while( !arq.eof()){
		palavra = linha.substr(0,linha.find(":") + 1);			//
		linha = linha.substr(linha.find(":") + 1);				//pular os labels
		if( palavra[palavra.size() - 1] == ':')					//
			saida << palavra << "\t";							//
		
		for(i=macroList->next;i != NULL; i = i->next)			//
			if( linha.find( i->name) != linha.npos)				//A cada linha do programa, verifica se contem algum nome de macro
				break;											//
				
		if(i == NULL){											//
			while(linha[0] == ' ' || linha[0] == '\t')			//Removendo ' ' e '\t' do inicio da linha, para tornar padrao
				linha = linha.substr( 1 );						//
			if(!linhaNula(linha))
				if( isOpcode( linha.substr(0, linha.find(" "))) < 0){
					cout << linha.substr(0, linha.find(" ")) << endl;
					cout << "EROOOOOOOOOOOOOOOOOO - macro chamada, mas nao definida" << endl;
					arq.close();
					saida.close();
					return 2;
				}
			saida << linha << endl;								//Se nao for chamada de macro, copiar a linha
		}
			
		else{
			while(linha[0] == ' ' || linha[0] == '\t')
				linha = linha.substr(1);
			linha = linha.substr( linha.find(" "));
			while(linha.find(" ") != linha.npos)
				linha = (linha.substr(0,linha.find(" ")) + linha.substr(linha.find(" ") + 1, linha.size()));  // SPACE SLAYER
			while(linha[0] != '\0'){
				inserirNode(listChamada, linha.substr(0,linha.find(",")));
				if(linha.find(",") != linha.npos)
					linha = linha.substr( linha.find(",") + 1);
				else
					linha = linha.substr( linha.size()   );
			}
			//listChamada pronta
			for(j = i->defList->next; j != NULL; j = j->next){
				
				linhaExpandida = j->str;

				if( linhaExpandida.find("MACRO") != linhaExpandida.npos){
					j = definir_macro_aninhada(i,j,listChamada);
				}
				else{
					linhaExpandida = substituiArgumentos(linhaExpandida,listChamada,i->argList);	
					saida << linhaExpandida << endl;
				}
			}
			
			listChamada->next = NULL;			
		}
	
		getline(arq, linha);
	}
	
	arq.close();
	saida.close();
	
	ifstream entrada("codigoExpandido.txt");
	ofstream saida2(codigoSaida);
	while(!entrada.eof()){
		entrada.get(c);
		if(c > 23 || c < 17)
			saida2 << c;
		
	}
	entrada.close();
	saida2.close();
	return 0;
}

node* definir_macro_aninhada(macro *ma, node *macroDef,node *listChamada){					//Define uma macro aninhada 
	node *paramReais = ma->argList->next;
	node *paramVirtuais = listChamada->next;
	node *j;
	macro *i,*macro_novo;
	int nivel = 0;
	string nome,linha,argumento;
	j = macroDef;
	
	linha = j->str;
	nome = linha.substr( 0, linha.find(" "));
	linha = linha.substr( linha.find("MACRO") + 5);
	for(i = macroList->next; i != NULL; i = i->next){
		if(nome == i->name)
			break;
	}
	
	if(i != NULL){
		i->argList->next = NULL;
		i->defList->next = NULL;
		macro_novo = i;
	}

	else{
		macro_novo = inserirMacro( macroList, nome);
		macro_novo->argList = (node*) malloc( sizeof(node));
		macro_novo->argList->next = NULL;
		macro_novo->defList = (node*) malloc(sizeof(node));
		macro_novo->defList->next = NULL;
	}
		
	while(linha.find(" ") != linha.npos)
		linha = (linha.substr(0,linha.find(" ")) + linha.substr(linha.find(" ") + 1, linha.size()));  // SPACE SLAYER
	while(linha[0] != '\0'){
		argumento = linha.substr(0,linha.find(","));
		inserirNode(macro_novo->argList, argumento);
		if(linha.find(",") != linha.npos)
			linha = linha.substr( linha.find(",") + 1);
		else
			linha = linha.substr( linha.size()   );
	}
	j = j->next;
	linha = j->str;
			
		
	while (linha.find("MEND") == linha.npos || nivel != 0){     
		while (linha[0] == ' ' || linha[0] == '\t')
			linha = linha.substr(1);
			if(linha.find("MACRO") != linha.npos)
				nivel++;
			if(linha.find("MEND") != linha.npos)
				nivel--;
			
			linha = substituiArgumentos( linha, listChamada, ma->argList);
			
			inserirNode( macro_novo->defList, linha);
		
		j = j->next;
		linha = j->str;
					
	}
	
	return j;	
}

string substituiArgumentos(string linha,node *paramReais,node *paramVirtuais){				//Substitui os parametros virtuais pelos parametros reais dentro de uma linha
	

	node *i,*j,*k;
	string argumento;
	j = paramReais->next;
	

	for(i=paramVirtuais->next;i != NULL; i = i->next){
		argumento = i->str;
		if(linha.find(argumento) != linha.npos)
			linha = (linha.substr( 0, linha.find(argumento)) + j->str ) + (linha.substr( linha.find(argumento) + argumento.size() , linha.size() ));
		j = j->next;
	}

	return linha;
}

bool linhaNula(string linha){																//Recebe uma linha, retorna true se ela possui só " " e "/t", false caso contrario
	bool saida = false;
	while(linha.find(" ") != linha.npos)
		linha = (linha.substr(0,linha.find(" ")) + linha.substr(linha.find(" ") + 1, linha.size()));
	while(linha.find("\t") != linha.npos)
		linha = (linha.substr(0,linha.find("\t")) + linha.substr(linha.find("\t") + 1, linha.size()));
	if(linha[0] == '\0')
		saida = true;
	
	return saida;
}

int definir_macro(string linha,ifstream *arq){ 												//Recebe a primeira linha da definição de macro e o arquivo a ser lido, cria uma nova macro
	macro *i,*macro_novo;
	int nivel = 0;
	string nome, argumento;
	while(linha[0] == ' ' || linha[0] == '\t')
		linha = linha.substr( 1 );

	nome = linha.substr( 0, linha.find(" "));
	
	macro_novo = inserirMacro( macroList, nome);
	
	macro_novo->argList = (node*) malloc( sizeof(node));
	macro_novo->argList->next = NULL;

	linha = linha.substr( linha.find("MACRO") + 5);

	while(linha.find(" ") != linha.npos)
		linha = (linha.substr(0,linha.find(" ")) + linha.substr(linha.find(" ") + 1, linha.size()));  // SPACE SLAYER

	while(linha[0] != '\0'){
	
		if(linha[0] != '\0'){
			argumento = linha.substr(0,linha.find(","));

			inserirNode(macro_novo->argList, argumento);

			if(linha.find(",") != linha.npos)
				linha = linha.substr( linha.find(",") + 1);
			else
				linha = linha.substr( linha.size()   );
		}
	}
	macro_novo->defList = (node*) malloc(sizeof(node));
	macro_novo->defList->next = NULL;
	getline(*arq, linha);
			
	while(linha.find("MEND") == linha.npos || nivel != 0){     
		
		if((*arq).eof())							//  Macro mal definida, faltou o MEND
			return 3;							//
		
		while(linha[0] == ' ' || linha[0] == '\t')
			linha = linha.substr(1);
		if(linha.find("MACRO") != linha.npos)
			nivel++;
		if(linha.find("MEND") != linha.npos)
			nivel--;
			
		inserirNode(macro_novo->defList, linha);
		getline(*arq, linha);	
	}
}

bool listaVazia(node *no){																	//Recebe a cabeça de uma lista de nodos, retorna true se tiver vazia, false caso contrario
	if(no->next == NULL)
		return true;
	return false;	
}

node* inserirNode(node *no, string txt){													//Recebe a cabeça de alguma lista de nodes e o nome da chave a ser inserida, retorna o endereço do novo node
	 int tamanho;
	 node *p;
	 string aux;
	 node *novo = (node* )malloc( sizeof(node));
	 if( novo == NULL){
		cout << "INSERIR: SEM MEMORIA - PERDEU VACILAO" << '\n';
	 }
	 tamanho = txt.size();
	 txt.copy(novo->str,txt.size(),0);
	novo->str[tamanho] = '\0';

	 novo->next = NULL;
	 p = no;
	 while(p->next != NULL){
		 p = p->next;
	 }	
	 p->next = novo;	
	return novo;
}

macro* inserirMacro(macro *ma, string name){												//Recebe a cabeça da lista de macros e o nome da macro a ser inserida, retorna o endereço da nova macro
	int tamanho;
	 macro *p;
	 string aux;
	 macro *novo = (macro* )malloc( sizeof(macro));
	 if( novo == NULL){
		cout << "INSERIR: SEM MEMORIA - PERDEU VACILAO" << '\n';
	 }
	 tamanho = name.size(); 
	 name.copy(novo->name,name.size(),0);
	novo->name[tamanho] = '\0';

	 novo->next = NULL;
	 p = ma;
	 while(p->next != NULL){
		 p = p->next;
	 }	
	 p->next = novo;	
	return novo;
}

void imprime(node *no){																		//Recebe a cabeça de uma lista encadeada de nodes e os imprime
		node *p;
		p = no->next;
		while(p != NULL){
			cout << p->str << '\n';
			p = p->next;
		}
}

int isOpcode(string leitura){
    if(leitura[0] == '+')
		leitura = leitura.substr(1);
    if (leitura.compare("AND") == 0)
        return 1;
    else if (leitura.compare("OR") == 0)
        return 1;
	else if (leitura.compare("MUL") == 0)
		return 1;
    else if (leitura.compare("CLEAR") == 0)
        return 1;
    else if (leitura.compare("COMP") == 0)
        return 1;
    else if (leitura.compare("COMPR") == 0)
        return 1;
    else if (leitura.compare("SHIFTR") == 0)
        return 1;
    else if (leitura.compare("SHIFTL") == 0)
        return 1;
    else if (leitura.compare("ADDR") == 0)
        return 1;
    else if (leitura.compare("ADD") == 0)
        return 1;
    else if (leitura.compare("RMO") == 0) 
        return 1;
    else if (leitura.compare("DIV") == 0)
        return 1;
    else if (leitura.compare("DIVR") == 0)
        return 1;
    else if (leitura.compare("MULR") == 0)
        return 1;
    else if (leitura.compare("SUB") == 0)
        return 1;
    else if (leitura.compare("SUBR") == 0)
        return 1;
    else if (leitura.compare("J") == 0)
        return 1;
    else if (leitura.compare("JEQ") == 0)
        return 1;
    else if (leitura.compare("JGT") == 0)
        return 1;
    else if (leitura.compare("JLT") == 0)
        return 1;
    else if (leitura.compare("JSUB") == 0)
        return 1;
    else if (leitura.compare("RSUB") == 0)
        return 1;
    else if (leitura.compare("LDA") == 0)
        return 1;
    else if (leitura.compare("LDCH") == 0)
        return 1;
    else if (leitura.compare("LDX") == 0)
        return 1;
    else if (leitura.compare("LDL") == 0)
        return 1;
    else if (leitura.compare("LDB") == 0)
        return 1;
    else if (leitura.compare("LDS") == 0)
        return 1;
    else if (leitura.compare("LDT") == 0)
        return 1;
    else if (leitura.compare("STA") == 0)
        return 1;
    else if (leitura.compare("STX") == 0)
        return 1;
    else if (leitura.compare("STL") == 0)
        return 1;
    else if (leitura.compare("STB") == 0)
        return 1;
    else if (leitura.compare("STS") == 0)
        return 1;
    else if (leitura.compare("STT") == 0)
        return 1;
    else if (leitura.compare("STCH") == 0)
        return 1;
    else if (leitura.compare("STSW") == 0)
        return 1;
    else if (leitura.compare("HLT") == 0)
        return 1;
	else if (leitura.compare("BYTE") == 0)
		return 1;
	else if (leitura.compare("WORD") == 0)
		return 1;
	else if (leitura.compare("EXTDEF") == 0)
		return 1;
	else if (leitura.compare("EXTREF") == 0)
		return 1;
    else
        return -1;      
}
