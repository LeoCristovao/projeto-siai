#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

char opcao_menu() {
    system("clear");
    cout << " (M)ovimentar robo" << endl;
    cout << " (R)ealizar testes" << endl;
    cout << " (S)intonizar" << endl;
    cout << " (F)im" << endl;
    cout << "> ";
    return (toupper(getchar()));
}

void movimentar_robo() {
    int tecnica;
    float posicao;

    system("clear");
    cout << "Selecione a técnica de controle:" << endl;
    cout << " (1) PID" << endl;
    cout << " (2) PID Adaptativo" << endl;
    cout << " (3) Fuzzy" << endl;
    cout << "> ";
    cin >> tecnica;

    switch(tecnica) {
        case 1:
            cout << "Técnica PID selecionada" << endl;
        break;
        case 2:
            cout << "Técnica PID Adaptativo selecionada" << endl;
        break;
        case 3:
            cout << "Técnica Fuzzy selecionada" << endl;
        break;
    }

    cout << " Entre com a posição desejada em milímetros " << endl;
    cout << "> ";
    cin >> posicao;
    system("clear");

    cout << "Iniciando movimentação do robo para a posição " << posicao << "mm" << endl;
    system("sleep 2s");
    
    int distanciaRestante = posicao;
    for(int i=0; i <= posicao; i++) {
        cout << "Faltam " << distanciaRestante << "mm " << "para a posição desejada (" << posicao << "mm)"  << endl;
        system("sleep 1s");
        system("clear");
        distanciaRestante = distanciaRestante - 1;
    }

    cout << "Posição alcançada com sucesso!" << endl;
    system("sleep 3s");
}

void realizar_testes() {
    cout << "realizando testes...." << endl;
}

void sintonizar() {
    cout << "sintonizando o robo...." << endl;

}

int main() {

    char op;

    do
    {
        op = opcao_menu();
        switch(op)
        {
            case 'M':
                movimentar_robo();
            break;

            case 'R':
                realizar_testes();
            break;

            case 'S':
                sintonizar();
            break;
        }
        cout << endl;
    } while(op != 'F');

    return 0;

}