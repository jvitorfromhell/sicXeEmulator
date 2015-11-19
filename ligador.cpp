#include <iostream>
#include <streambuf>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <iomanip>

using namespace std;

struct No{
		string str;
		int endereco;
};
		
typedef struct No no;

int procuraEndereco(string nome);

no TSG[40];

int ligador(string montado1, string montado2, string tabUso1, string tabUso2, string tabDef1, string tabDef2, string codigoCarregavel){

	ifstream modulo1(montado1);
	ifstream modulo2(montado2);
	ifstream tabelaDefinicao1(tabDef1);
	ifstream tabelaDefinicao2(tabDef2);
	ifstream tabelaUso1(tabUso1);
	ifstream tabelaUso2(tabUso2);
	ofstream intermediario( "intermediario.txt");
	ofstream saida(codigoCarregavel);
	int aux,aux2,tamanho=0,i,j,endereco=0;
	int operando,tam,tamanhoTSG=0,tamanhoUSO=0;
	string linha,auxiliar,reloca;
	
	while(!modulo1.eof()){
			
			modulo1 >> aux;

			
			getline( modulo1, linha);
			if (!modulo1.eof()){
				intermediario << aux << linha << endl;
				tamanho = tamanho + aux;
			}
	}
	
	while( !modulo2.eof()){
		modulo2 >> tam;
		if (!modulo2.eof())
			intermediario << tam ;
		if(tam == 3 || tam == 4){
			modulo2 >> hex >> reloca;
			intermediario  << " " << reloca;
			if(reloca == "01"){
				modulo2 >> hex >> auxiliar;
				intermediario  << " " << auxiliar;
				modulo2 >> auxiliar;
				intermediario << " " << auxiliar;
				modulo2 >> hex >> operando;
				if(operando >= 0)
					operando = operando + tamanho;
				if(operando == -1)						
					intermediario << " " << -1;
				else
					intermediario << " " << hex << setfill('0') << setw(2) << operando;
			}
		}
		
		getline(modulo2, linha);
		if (!modulo2.eof())
			intermediario << linha << endl;	
		tam = 0; reloca = "0";
		
	}

	intermediario.close();
	
	
	
	i=0;
	while( !tabelaDefinicao1.eof() ){
		
		tabelaDefinicao1 >> TSG[i].str;
		tabelaDefinicao1 >> TSG[i].endereco;
		tamanhoTSG++;
		i++;
	}
	i--;tamanhoTSG--;
	while( !tabelaDefinicao2.eof() ){
		tabelaDefinicao2 >> TSG[i].str;
		tabelaDefinicao2 >> TSG[i].endereco;
		TSG[i].endereco += tamanho;
		tamanhoTSG++;
		i++;
	}
	i--;tamanhoTSG--;
	
	/*
	for(i=0;i<tamanhoTSG;i++){
		cout << TSG[i].str << ": " << TSG[i].endereco << endl;
	}
	*/
	
	
	no USO[40];
	j=0;
	while( !tabelaUso1.eof() ){
		
		tabelaUso1 >> USO[j].str;
		tabelaUso1 >> USO[j].endereco;
		if(USO[j].endereco == -2){
			j--;
			tamanhoUSO--;
		}
		tamanhoUSO++;
		j++;
		
	}
	j--;tamanhoUSO--;
	
	while( !tabelaUso2.eof() ){
		tabelaUso2 >> USO[j].str;
		tabelaUso2 >> USO[j].endereco;
		USO[j].endereco += tamanho;
		if(USO[j].endereco == tamanho-2){
			j--;
			tamanhoUSO--;
		}
		
		tamanhoUSO++;
		j++;
		
	}
	j--;tamanhoUSO--;
	
	/*
	for(j=0;j<tamanhoUSO;j++){
		cout << USO[j].str << ": " << USO[j].endereco << endl;
	}
	*/
	
	ifstream inter( "intermediario.txt" );
	
	for(i=0;i<tamanhoUSO;i++){
		
		do{
			inter >> aux;
			endereco += aux;
			saida << aux;
			
			if(endereco < USO[i].endereco){
				getline( inter, linha);
	
				saida << linha << endl;
			}
		}while( endereco < USO[i].endereco);
		inter >> reloca;
		saida  << " " << reloca;
		inter >> hex >> auxiliar;
		saida  << " " << auxiliar;
		inter >> auxiliar;
		saida << " " << auxiliar;
		inter >> hex >> operando;
		operando = procuraEndereco(USO[i].str);
		saida << " " << hex << setfill('0') << setw(2) << operando;
		getline(inter, linha);
		saida << linha << endl;	
		
	}
	while( !inter.eof()){
		getline(inter,linha);
		saida << linha << endl;
	}
	
	modulo1.close();
	modulo2.close();
	tabelaDefinicao1.close();
	tabelaDefinicao2.close();
	tabelaUso1.close();
	tabelaUso2.close();
	saida.close();
	inter.close();
	
return 0;
}


int procuraEndereco(string nome){
	int i=0;
	while(1){
		
		if(TSG[i].str == nome)
			return TSG[i].endereco;
		i++;
	}
	

}


