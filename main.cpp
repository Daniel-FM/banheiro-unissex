// Exercício:

// - Há um banheiro unissex com X boxes
// - Y pessoas vão usar o banheiro
// - Em intervalos de tempo aleatórios, uma pessoa (homem ou mulher, 50% de chance cada) entra na fila
// - Se houver pelo menos um box livre quando a pessoa está na primeira posição da fila, ela imediatamente entra no banheiro, ocupando um box livre
// - A pessoa usa o box por um tempo aleatório, depois imediatamente o desocupa
// - Homens só podem entrar se não houver nenhum box ocupado por mulheres, e vice-versa

#include <thread>
#include <mutex>
#include <iostream>
#include <random>
#include <string.h>
#include <list>
#include <array>
#include <condition_variable>
#include <atomic>

using namespace std;

constexpr int SEM_BOX_DISP = -1;

constexpr int TOTAL_PESSOAS = 20;
constexpr int NUM_BOXES = 4;

atomic<int> qtdPessoasQueUsaram = 0;
array<char, NUM_BOXES> boxes;
list<char> fila;

mt19937 mt(random_device{}());
uniform_int_distribution rand_genero(0, 1);
uniform_int_distribution rand_tempoFila(500, 1000);
uniform_int_distribution rand_tempoBox(2000, 8000);

mutex mut_print;
mutex mut_fila;
mutex mut_box;

condition_variable cv_filaNaoMaisVazia;
condition_variable cv_boxUsado;

void imprime_boxes(){
    for (char box : boxes){
        cout<<"|"<< box;
    }
    cout<<"|"<<endl;
}

void imprime_fila(){
    for (char pessoa : fila){
        cout<<pessoa;
    }
    cout<<endl;
}

void imprime(string str){
    cout<<str<<endl;
}

void usar_box(int boxSendoUsado){
    int tempoDeUso = rand_tempoBox(mt);
    this_thread::sleep_for(chrono::milliseconds(tempoDeUso));
    
    lock_guard lck1(mut_box);
    boxes[boxSendoUsado] = ' ';
    qtdPessoasQueUsaram++;

    imprime_boxes();
	cv_boxUsado.notify_one();
}

int pega_box_disponivel(char generoDaPessoaNoInicioDaFila){
	// Primeiro checa se tem alguém de genero diferente no banheiro
	for (int i = 0; i < NUM_BOXES; i++){
		if (boxes[i] != ' ' && boxes[i] != generoDaPessoaNoInicioDaFila){
			return SEM_BOX_DISP;
		}
	}
	// Se não tiver, pega o primero box livre que encontrar
	for (int i = 0; i < NUM_BOXES; i++){
		if (boxes[i] == ' '){
			return i;
		}
	}

	return SEM_BOX_DISP;
}

void popula_fila(){
    for (int i = 0; i < TOTAL_PESSOAS; i++){
        this_thread::sleep_for(chrono::milliseconds(rand_tempoFila(mt)));
        int novaPessoa = rand_genero(mt);
		bool filaEstavaVazia = false;

		lock_guard lck(mut_fila);
		filaEstavaVazia = fila.empty() ? true : false;
		fila.push_back(novaPessoa == 0 ? 'H' : 'M');
		imprime_fila();

		if (filaEstavaVazia){
			cv_filaNaoMaisVazia.notify_one();
		}
    }
    imprime("Chegaram todas as pessoas!");
}

void esvazia_fila(){
    vector<thread> usosDosBoxes;

    while (true){
		if (qtdPessoasQueUsaram >= TOTAL_PESSOAS){
			break;
		}

		char generoDaPessoaNoInicioDaFila;

		EsperaTerPeloMenosUmaPessoaNaFila:{
			unique_lock lck_esperaFilaTerGente(mut_fila);
			if (fila.empty() && cv_filaNaoMaisVazia.wait_for(lck_esperaFilaTerGente, 1s) == std::cv_status::timeout){
				// Ninguém na fila para tirar! Tenta de novo.
				continue;
			};
			generoDaPessoaNoInicioDaFila = fila.front();
		}
        
        int boxDisponivel = SEM_BOX_DISP;

		EsperaTerBoxDisponivel:{
			unique_lock lck_esperaBoxSerUsado(mut_box);
			boxDisponivel = pega_box_disponivel(generoDaPessoaNoInicioDaFila);
			// Se não tiver nenhum box disponível, fica checando sempre que um box for desocupado
			while (boxDisponivel == SEM_BOX_DISP){
				cv_boxUsado.wait(lck_esperaBoxSerUsado);
				boxDisponivel = pega_box_disponivel(generoDaPessoaNoInicioDaFila);
			}
		}

		TiraDaFila:{
			scoped_lock lck(mut_box, mut_fila);

			// Tira a pessoa da frente da fila e coloca no box disponível
			fila.pop_front();
			boxes[boxDisponivel] = generoDaPessoaNoInicioDaFila;
			
			imprime_fila();
			imprime_boxes();

			// Começa a usar o box em uma nova thread
			usosDosBoxes.emplace_back(thread(usar_box, boxDisponivel));
		}
    }

    for (auto& thr : usosDosBoxes){
        thr.join();
    }
}

int main()
{
    // Inicializa o array dos boxes
    boxes.fill(' ');

    thread t1(popula_fila);
    thread t2(esvazia_fila);
    
    t1.join();
    t2.join();

    imprime("FIM!");
}
