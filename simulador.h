#include <gtkmm.h>
#include <vector>
#include <bitset>
#include "SIC.h"

using namespace std;

int processadorMacro(string codigoEntrada, string codigoSaida);
int montador(string arquivoEntrada, string arquivoSaida, string tabDef, string tabUso);
int ligador(string montado1, string montado2, string tabUso1, string tabUso2, string tabDef1, string tabDef2, string codigoCarregavel);

class Simulador : public Gtk::Window {
	public:
		Simulador(int nArq);
		virtual ~Simulador();

	protected:
		Gtk::Box botoes;
		Gtk::Button executa, passo, macros, montar, ligar, carregar;
		Gtk::Paned vDiv, hDiv1, hDiv2, hDiv3, macroDiv, montaDiv;
		Gtk::Notebook mem_reg;
		Gtk::Grid registradores, memoria;
		Gtk::ScrolledWindow janela_inicial1, janela_inicial2, janela_montado1, janela_montado2, janela_codigo, janela_memoria;
		Gtk::TextView inicial1, inicial2, montado1, montado2, codigoFinal;
		Gtk::Label reg[10], regVal [10], mem[1000], memVal[1000], memTitulo, memTituloVal;
		Glib::RefPtr<Gtk::TextBuffer> bufferInicial1, bufferInicial2, bufferMacro1, bufferMacro2, bufferMontado1, bufferMontado2, bufferFinal, bufferErro;
		Glib::RefPtr<Gtk::TextBuffer::Tag> tag1;
		Glib::RefPtr<Gtk::TextBuffer::TagTable> tags;
		Glib::RefPtr<Gtk::TextBuffer::Mark> mark;
		SIC processador;
		unsigned char memSimulador[1000000];
		int nixbpe[1000000];
		int nArq, memIndex = 0, instIndex = 0;

		void click_executa();
		void click_passo();
		void click_macros();
		void click_montar();
		void click_ligar();
		void click_carregar();
		void update_registradores();
		void carregador(string arquivoLinkado);
		void setarMem(int pos, int val);
		void update_memoria();
};
