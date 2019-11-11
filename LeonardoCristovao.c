/*
    Trabalho 2 - CAP 2014, Prof. Dr. Ednaldo Brigante Pizzolato
    Autor: Leonardo de Andrade Cristovão
    Bibliografia: Primeiro Curso de programação em C, Edson Luiz França Senne.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Criação da estrutura cliente
typedef struct
{
    int nconta;          //Número da conta
    char nome[20];       //Nome do cliente
    float limite;        //Limite  em reais para efetuar saques
    float saldo;         //Saldo da conta corrente
    float saldoPoupanca; //Saldo da conta poupança
    char senha[5];       //Senha da conta do cliente
} cliente;
//Criação da estrutura gerente
typedef struct
{
    char senha[5];       //Senha do gerente que permite criação de novas contas
} gerente;

FILE *arq;
cliente cli;
gerente ger;
//Função Menu para selecionar operações por meio de um caracter
char opcao_menu()
{
    system("cls");
    printf(" (A)bertura de conta\n");
    printf(" (M)udar a senha\n");
    printf(" (L)istagem de saldo\n");
    printf(" (D)eposito\n");
    printf(" (S)aque\n");
    printf(" (T)ransferencia\n");
    printf(" (E)ntre contas\n");
    printf(" (F)im\n");
    printf("> ");
    return (toupper(getche())); //Captura o char no instante que ele é digitado(sem usar Enter), torna-o maiúsculo(caso não seja)
}
//Função que acessa sequencialmente os registros do tipo cliente para encontrar o número de uma conta
//Essa função presume que o ponteiro esteja inicialmente apontando para a primeira struct cliente no arquivo
int encontra_conta(int n)
{
    fread(&cli,sizeof(cliente),1,arq); //A cada fread, o ponteiro avança para o registro seguinte
    while(!feof(arq))
    {
        if(cli.nconta == n)
        {
            fseek(arq, -sizeof(cliente), SEEK_CUR); //Posiciona o ponteiro no início do registro que contenha o numero da conta desejada
            return 1; //Retorna 1 caso a conta seja encontrada
        }
        fread(&cli, sizeof(cliente), 1, arq);
    }
    return 0; //Retorna 0 caso a conta não seja encontrada
}
//Procedimento que captura uma senha de 4 digitos e imprime na tela asteriscos conforme se digita, e retorna essa senha por parâmetro
void obtem_senha(char *senha)
{
    int i;

    printf("Senha de acesso ..... ");
    for(i=0;i<4;i++)
    {
        senha[i] = getch();
        printf("*");
    }
    printf("\n");
}
//Procedimento que recebe por parâmetro uma senha a ser criptografada, retornando-a pelo segundo parâmetro
void cripto_senha(char *senha, char *novasenha)
{
    char *p, *pnova;
    int i=0;

    p = senha;
    pnova = novasenha;
    while(*p != '\0')
    {
        *pnova = *p+i;  //Método de criptografação
        pnova++;
        p++;
        i++;
    }
    *pnova = '\0';      //Coloca o indicador de final de frase na senha criptografada
}
//Procedimento que recebe por parâmetro uma senha a ser descriptografada, retornando-a pelo segundo parâmetro
void descripto_senha(char *senha, char *novasenha)
{
    char *p, *pnova;
    int i=0;

    p = senha;
    pnova = novasenha;
    while(*p != '\0')
    {
        *pnova = *p-i;  //Método de descriptografação
        pnova++;
        p++;
        i++;
    }
    *pnova = '\0';      //Coloca o indicador de final de frase na senha descriptografada
}
//Procedimento a ser chamado quando o cliente deseja mudar a senha de acesso da sua conta
void mudar_senha()
{
    int num;
    char senha[5], novasenha[5], mudasenha[5], novamudasenha[5];

    arq = fopen("banco.dat", "r+b"); //Abre o arquivo binário existente para leitura e gravação de dados
    if(arq == NULL)
    {
        printf("\nErro ao abrir o arquivo\n");
        return 1;
    }
    printf("\n");
    printf("Numero da conta ..... ");//Número da conta que se deseja mudar a senha
    scanf("%d", &num);
    if(encontra_conta(num) == 0)     //Verifica se a conta existe
        printf("\nEssa conta nao existe!\n");
    else
    {
        fread(&cli, sizeof(cliente), 1, arq);
        obtem_senha(senha);                          //Chamada do procedimento para capturar a senha que deseja ser mudada
        descripto_senha(cli.senha, novasenha);       //Chamada do procedimento para descriptografá-la
        if(!strcmp(novasenha, senha))                //Compara se a senha digitada e a senha descriptografada são iguais
        {
            obtem_senha(mudasenha);                  //Chamada do procedimento para capturar a nova senha desejada
            cripto_senha(mudasenha, novamudasenha);  //Chamada do procedimento para criptografá-la
            strcpy(cli.senha, novamudasenha);        //Copia a senha criptografada na senha do cliente
            fseek(arq, -sizeof(cliente), SEEK_CUR);  //Posiciona o ponteiro no início do registro desejado
            fwrite(&cli, sizeof(cliente), 1, arq);   //Escreve no arquivo binário a modificação
            printf("Senha alterada.\n");
        }
        else
            printf("Senha incorreta.");
    }
    fclose(arq);
}
//Procedimento exclusivo para privilégio do gerente que permite a criação de novas contas
void abertura_conta()
{
    int num;
    char senha[5], novasenha[5], senhaGerente[5], novasenhaGerente[5];

    arq = fopen("banco.dat", "r+b");     //Abre o arquivo binário existente para leitura e gravação de dados
    if(arq == NULL)                      //Se o arquivo não existir, então o gerente deve assumir controle para criação da primeira conta
    {
        //Criacao da senha do gerente
        arq = fopen("banco.dat", "a+b"); //Abre o arquivo binário para anexação de dados, se o arquivo não existir, será criado um novo arquivo
        if(arq == NULL)
            exit(1);
        printf("\nPrimeiro acesso. \nCrie a senha do gerente:\n");
        obtem_senha(senhaGerente);                    //Captura para criar a senha do gerente
        cripto_senha(senhaGerente, novasenhaGerente); //Criptografa a senha digitada e retorna-a por parâmetro
        strcpy(ger.senha, novasenhaGerente);          //Copia a senha criptografada na variável senha do gerente
        fwrite(&ger, sizeof(gerente), 1, arq);        //Escreve no arquivo a senha criptografada
        printf("Senha criada.\n");
    }
    printf("\n");                                     //Assume-se que se chegou nesse ponto, já existe um arquivo e o gerente já possui uma senha
    printf("Digite a senha do gerente:\n");           //Pedindo a senha do gerente para que ele possa criar uma nova conta
    obtem_senha(senhaGerente);                        //Captura a senha digitada
    fread(&ger, sizeof(gerente), 1, arq);             //Le no arquivo a estrura gerente
    descripto_senha(ger.senha, novasenhaGerente);     //Procedimento que descriptografa a senha lida no arquivo binário e retorna-a por parâmetro
    if(!strcmp(senhaGerente, novasenhaGerente))       //Compara se a senha digitada é a mesma da senha descriptografaca
    {
        arq = fopen("banco.dat", "r+b");              //Abre o arquivo binário existente para leitura e gravação de dados
        fseek(arq, sizeof(cliente), SEEK_SET);        //Posiciona o ponteiro para o ínicio da primeira estrutra cliente no arquivo binário
        printf("Escolha o numero da conta: ");
        scanf("%d", &num);
        if (encontra_conta(num) == 1)                 //Verifica se a conta já existe
            printf("\nEssa conta ja existe!\n");
        else
        {
            cli.nconta = num;
            printf("Digite os seguintes dados:\n");
            printf("  Nome do cliente ..... ");
            fflush(stdin);
            gets(cli.nome);                           //Captura o nome do cliente até que seja pressionada a tecla enter
            printf("  ");
            obtem_senha(senha);
            cripto_senha(senha, novasenha);
            strcpy(cli.senha, novasenha);
            printf("  Limite de credito ... ");
            scanf("%f", &cli.limite);
            printf("  Saldo inicial ....... ");
            scanf("%f", &cli.saldo);
            fwrite(&cli, sizeof(cliente), 1, arq);    //Escreve no arquivo binário a nova conta do cliente
            printf("Conta %d aberta.\n", num);
        }
    }
    else
        printf("Senha incorreta.");
    fclose(arq);
}
//Procedimento que faz a leitura do arquivo binário para mostrar na tela o saldo de uma conta
void listagem_saldo()
{
    int num;
    char senha[5], novasenha[5];

    arq = fopen("banco.dat","r+b"); //Abre o arquivo binário existente para leitura e gravação de dados
    if(arq == NULL)
    {
        printf("\nErro ao abrir o arquivo\n");
        return 1;
    }
    printf("\n");
    printf("Numero da conta ..... ");
    scanf("%d", &num);
    if(encontra_conta(num) == 0)
        printf("\nEssa conta nao existe!\n");
    else
    {
        fread(&cli,sizeof(cliente),1,arq);
        obtem_senha(senha);
        descripto_senha(cli.senha, novasenha);
        if(!strcmp(novasenha, senha))
        {
            printf("\n");
            printf("+--------+-----------------------+-----------+--------------+--------------+\n");
            printf("| CONTA  | CLIENTE               |  CREDITO  |   CORRENTE   |   POUPANCA   |\n");
            printf("+--------+-----------------------+-----------+--------------+--------------+\n");
            printf("| %05d  | %20s  | %9.2f | %12.2f | %12.2f |\n", cli.nconta, cli.nome, cli.limite, cli.saldo, cli.saldoPoupanca);
            printf("+--------+-----------------------+-----------+--------------+--------------+\n");
        }
        else
            printf("Senha incorreta.");
    }
    fclose(arq);
}
//Função que escolhe entre conta Corrente ou conta Poupança
char escolheConta()
{
    char tipoConta;
    printf("Conta:\n (C)orrente\n (P)oupanca\n");
    printf("> ");
    tipoConta = toupper(getche());
    while(tipoConta != 'C' && tipoConta != 'P')
    {
        printf("\nEntre apenas com:\n (C)orrente\n (P)oupanca:\n");
        printf("> ");
        tipoConta = toupper(getche());
    }
    return tipoConta;    //Retorna um char C para conta Corrente ou P para conta Poupança
}
//Procedimento para realização de depósito em conta corrente ou conta poupança
void deposito()
{
    int num;
    float valor;
    char tipoConta;
    char senha[5], novasenha[5];

    arq = fopen("banco.dat", "r+b"); //Abre o arquivo binário existente para leitura e gravação de dados
    if(arq == NULL)
    {
        printf("\nErro ao abrir o arquivo\n");
        return 1;
    }
    printf("\n");
    printf("Numero da conta ..... ");
    scanf("%d", &num);
    if(encontra_conta(num) == 0)
        printf("\nEssa conta nao existe!\n");
    else
    {
        fread(&cli, sizeof(cliente), 1, arq);
        obtem_senha(senha);                     //Captura a senha digitada
        descripto_senha(cli.senha, novasenha);  //Descriptografa a senha que está escrita no arquivo
        if(!strcmp(novasenha, senha))           //Compara a senha digitada com a senha descriptografada
        {
            printf("Valor do deposito ... ");
            scanf("%f", &valor);
            while(valor < 0)
            {
                printf("Esse valor nao corresponde a deposito, tente novamente:\n");
                printf("Valor do deposito ... ");
                scanf("%f", &valor);
            }
            if(valor == 0)
                printf("Nada foi alterado.");
            else
            {
                tipoConta = escolheConta();     //Chamada da função para escolher se o depósito será feito na conta corrente ou conta poupança
                if(tipoConta == 'C')
                {
                    cli.saldo = cli.saldo + valor;                 //Deposita na conta corrente
                    printf("\nDeposito efetuado.\n");
                }
                else
                {
                    cli.saldoPoupanca = cli.saldoPoupanca + valor; //Deposita na conta poupança
                    printf("\nDeposito efetuado.\n");
                }
                fseek(arq, -sizeof(cliente), SEEK_CUR);            //Posiciona o ponteiro no início do registro desejado
                fwrite(&cli, sizeof(cliente), 1, arq);             //Escreve no arquivo binário a modificação
            }
        }
        else
            printf("Senha incorreta.");
    }
    fclose(arq);
}
//Procedimento para realizar saque em conta corrente ou conta poupança
void saque()
{
    int num;
    float valor;
    char tipoConta;
    char senha[5], novasenha[5];

    arq = fopen("banco.dat", "r+b"); //Abre o arquivo binário existente para leitura e gravação de dados
    if(arq == NULL)
    {
        printf("\nErro ao abrir o arquivo\n");
        return 1;
    }
    printf("\n");
    printf("Numero da conta ..... ");
    scanf("%d", &num);
    if(encontra_conta(num) == 0)
        printf("\nEssa conta nao existe!\n");
    else
    {
        fread(&cli, sizeof(cliente), 1, arq);
        obtem_senha(senha);                     //Captura a senha digitada
        descripto_senha(cli.senha, novasenha);  //Descriptografa a senha escrita no arquivo
        if(!strcmp(novasenha, senha))           //Compara a senha digitada com a senha descriptografada
        {
            printf("Valor do saque ...... ");
            scanf("%f", &valor);
            while(valor < 0)
            {
                printf("Esse valor nao corresponde a saque, tente novamente:\n");
                printf("Valor do saque ...... ");
                scanf("%f", &valor);
            }
            if(valor == 0)
                printf("Nada foi alterado.");
            else
            {
                tipoConta = escolheConta();    //Escolhe entre conta corrente ou conta poupança
                if(tipoConta == 'C')
                    if(cli.saldo + cli.limite >= valor) //Verifica se é possível sacar aquele valor
                    {
                        cli.saldo = cli.saldo - valor;
                        printf("\nSaque efetudado.\n");
                    }
                    else
                        printf("\nSaque excede o limite (MAX = %.2f)\n", cli.saldo+cli.limite); //Mostra qual a quantidade máxima que pode ser sacada
                else
                {
                    if(cli.saldoPoupanca + cli.limite >= valor)
                    {
                        cli.saldoPoupanca = cli.saldoPoupanca - valor;
                        printf("\nSaque efetuado.\n");
                    }
                    else
                        printf("\nSaque excede o limite (MAX = %.2f)\n", cli.saldo+cli.limite); //Mostra qual a quantidade máxima que pode ser sacada
                }
                fseek(arq, -sizeof(cliente), SEEK_CUR); //Posiciona o ponteiro no início da estrutura desejada
                fwrite(&cli, sizeof(cliente), 1, arq);  //Escreve no arquivo binário a estrutura modificada acima
            }
        }
        else
            printf("Senha incorreta.");
    }
    fclose(arq);
}
//Procedimento para transferência entre conta corrente e conta poupança de um determinado cliente
void transferencia()
{
    int num;
    float valor;
    char tipoTransferencia;
    char senha[5], novasenha[5];

    arq = fopen("banco.dat", "r+b");  //Abre o arquivo binário existente para leitura e gravação de dados
    if(arq == NULL)
    {
        printf("\nErro ao abrir o arquivo\n");
        return 1;
    }
    printf("\n");
    printf("Numero da conta ..... ");
    scanf("%d", &num);
    if(encontra_conta(num) == 0)
        printf("\nEssa conta nao existe!\n");
    else
    {
        fread(&cli, sizeof(cliente), 1, arq);   //Lê a estrutura cliente
        obtem_senha(senha);                     //Captura a senha digitada
        descripto_senha(cli.senha, novasenha);  //Descriptografa a senha do cliente que está escrita no arquivo
        if(!strcmp(novasenha, senha))           //Compara a senha digitada com a senha descriptografada
        {
            printf("Valor da transf ..... ");   //Captura o valor a ser transferido
            scanf("%f", &valor);
            while(valor < 0)
            {
                printf("Esse valor nao corresponde a transferencias, tente novamente:\n");
                printf("Valor da transf ..... ");
                scanf("%f", &valor);
            }
            printf(" (A)De poupanca para corrente\n"); //Digita a para transferência de conta poupança para corrente
            printf(" (B)De corrente para poupanca\n"); //Digita b para transferência de conta corrente para poupança
            printf("> ");
            tipoTransferencia = toupper(getche());     //Captura sem a necessidade de pressionar Enter
            while(tipoTransferencia != 'A' && tipoTransferencia != 'B')
            {
                printf("\nEntre apenas com A ou B: ");
                tipoTransferencia = toupper(getche());
            }
            if(tipoTransferencia == 'A')
            {
                cli.saldoPoupanca = cli.saldoPoupanca - valor;  //Retira o valor da conta Poupança
                cli.saldo = cli.saldo + valor;                  //Coloca o valor na conta Corrente
                printf("\nTransferencia efetuada.\n");
            }
            else
            {
                cli.saldo = cli.saldo - valor;                 //Retira o valor da conta corrente
                cli.saldoPoupanca = cli.saldoPoupanca + valor; //Coloca o valor na conta poupança
                printf("\nTransferencia efetuada.\n");
            }
            fseek(arq, -sizeof(cliente), SEEK_CUR); //Posiciona o ponteiro no início da estrutura a ser modificada
            fwrite(&cli, sizeof(cliente), 1, arq);  //Escreve no arquivo as modificações feitas na operação acima
        }
        else
            printf("Senha incorreta.");
    }
    fclose(arq);
}
//Procedimento da retirada de um valor da conta corrente que será transferido para outra conta
void saque_para_transferencia(int *num, float *valor, int *limite)
{
    char senha[5], novasenha[5];
    char tipoTransferencia;

    arq = fopen("banco.dat", "r+b"); //Abre o arquivo binário existente para leitura e gravação de dados
    if(arq == NULL)
    {
        printf("\nErro ao abrir o arquivo\n");
        return 1;
    }
    printf("\n");
    printf("Numero da conta ..... ");
    scanf("%d", num);                 //Escolhe a conta na qual será retirado o dinheiro a ser transferido
    if(encontra_conta(*num) == 0)
        printf("\nEssa conta nao existe!\n");
    else
    {
        fread(&cli, sizeof(cliente), 1, arq);  //Lê a estrutura do cliente que tranfirirá
        obtem_senha(senha);                    //Captura a senha digitada
        descripto_senha(cli.senha, novasenha); //Descriptografa a senha do cliente, gravado no arquivo binário
        if(!strcmp(novasenha, senha))          //Compara a senha digitada com a senha descriptografada
        {
            printf("Valor da transf ..... ");
            scanf("%f", valor);
            while(*valor < 0)
            {
                printf("Esse valor nao corresponde a saque, tente novamente:\n");
                printf("Valor da transf ..... ");
                scanf("%f", valor);
            }
            if(*valor == 0)
                printf("Nada foi alterado.");
            else
            {
                if(*valor < 1000) //Condição para ser transferencia do tipo DOC
                    printf("A transferencia sera do tipo DOC.");
                else
                    if(*valor < 5000) //Condição que permite escolha entre transferência do tipo DOC ou tipo TED
                    {
                        printf("Tipo de transferencia:\n");
                        printf(" (D)OC\n");
                        printf(" (T)ED\n");
                        printf("> ");
                        tipoTransferencia = toupper(getche());  //Captura sem a necessidade de Enter a opção desejada
                        if(tipoTransferencia == 'D')
                            printf("A transferencia sera do tipo DOC.");
                        else
                            if(tipoTransferencia == 'C')
                                printf("A transferencia sera do tipo TED");
                    }
                    else
                        printf("A transferencia sera do tipo TED."); //Condição para transferencia ser do tipo TED
                if(cli.saldo + cli.limite >= *valor)  //Verifica se é possível fazer o saque para transferência
                    cli.saldo = cli.saldo - *valor;
                else
                {
                    printf("\nSaque excede o limite (MAX = %.2f)\n", cli.saldo+cli.limite);  //Mostra na tela o limite máximo que pode ser sacado
                    *limite = 1;  //Retorna por parâmetro caso o limite seja ultrapassado
                }
                fseek(arq, -sizeof(cliente), SEEK_CUR);  //Posiciona o ponteiro no início da estrutura cliente
                fwrite(&cli, sizeof(cliente), 1, arq);   //Escreve as modificações realizadas acima na estrutura cliente
            }
        }
        else
            printf("Senha incorreta.");
    }
    fclose(arq);
}
//Procedimento de depósito de um valor vindo de uma transferencia na conta corrente ou poupança
void deposito_para_transferencia(float valor, int *retorno)
{
    int num;
    char tipoConta;

    arq = fopen("banco.dat", "r+b"); //Abre o arquivo binário existente para leitura e gravação de dados
    if(arq == NULL)
        return 1;

    printf("\n");
    printf("Conta destino ....... ");
    scanf("%d", &num);                 //Escolhe a conta na qual será depositado a transferencia
    if(encontra_conta(num) == 0)
    {
        printf("\nEssa conta nao existe!\n");
        *retorno = 1;  //Retorna por parâmetro caso a conta destino não exista
    }
    else
    {
        fread(&cli, sizeof(cliente), 1, arq);  //Lê a estrutura do cliente destino
        tipoConta = escolheConta();  //Escolhe entre conta corrente ou conta poupança
        if(tipoConta == 'C')
        {
            cli.saldo = cli.saldo + valor; //Deposita na conta corrente
            printf("\nTransferencia efetuada.\n");
        }
        else
        {
            cli.saldoPoupanca = cli.saldoPoupanca + valor;  //Deposita na conta poupança
            printf("\nTransferencia efetuada.\n");
        }
        fseek(arq, -sizeof(cliente), SEEK_CUR);  //Posiciona o ponteiro no início da estrutura a ser modificada
        fwrite(&cli, sizeof(cliente), 1, arq);   //Escreve no arquivo binário as alterações feitas na conta destino
    }
    fclose(arq);
}
//Procedimento para caso o limite a ser transferido é ultrapassado, ou caso a conta destino não existir
void deposita_de_volta(int num, float valor)
{
    arq = fopen("banco.dat", "r+b"); //Abre o arquivo binário existente para leitura e gravação de dados
    if(arq == NULL)
    {
        printf("\nErro ao abrir o arquivo\n");
        return 1;
    }
    if(encontra_conta(num) == 0)
        printf("\nEssa conta nao existe!\n");
    else
    {
        fread(&cli, sizeof(cliente), 1, arq);   //Lê a estrutura do cliente origem
        cli.saldo = cli.saldo + valor;          //Deposita o dinheiro que não pode ser transferido
        printf("\nTransferencia retornada.\n");
        fseek(arq, -sizeof(cliente), SEEK_CUR); //Posiciona o ponteiro no início da estrutura do cliente a ser modificado
        fwrite(&cli, sizeof(cliente), 1, arq);  //Escreve no arquivo binário as alterações feitas no procedimento
    }
    fclose(arq);
}
//Procedimento para transferencia entre contas
void transferencia_entre_contas()
{
    int contaOrigem;
    float valorTransferencia;
    int retorno=0;
    int limite=0;

    //Retorna por parâmetro o numero da conta que transfere, o valor transferido e se o limite foi atingido ou não.
    saque_para_transferencia(&contaOrigem, &valorTransferencia, &limite);
    if(limite == 0) //Se o limite não foi atingido, então executa-se o procedimento para depositar o valor na conta destino
        deposito_para_transferencia(valorTransferencia, &retorno); //Retorna se a conta destino existe ou não
    else
        printf("Transferencia nao efetuada.");
    if(retorno == 1) //Se a conta não existir, então executa-se o procedimento para retornar o valor na conta origem
        deposita_de_volta(contaOrigem, valorTransferencia);
}


int main(int args, char * arg[])
{
    char op;

    do
    {
        //Ferramenta para escolher as opcões do menu
        op = opcao_menu();
        switch(op)
        {
            case 'A':
                abertura_conta();
            break;

            case 'M':
                mudar_senha();
            break;

            case 'L':
                listagem_saldo();
            break;

            case 'D':
                deposito();
            break;

            case 'S':
                saque();
            break;

            case 'T':
                transferencia();
            break;
            case 'E':
                transferencia_entre_contas();
            break;
        }
        printf("\n");
        system("pause");
    } while(op != 'F');
    //Executa-se pelo menos uma vez a escolha das operações até que seja pressionado f para finalizar.

    return 0;
}
