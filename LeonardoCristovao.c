/*
    Trabalho 2 - CAP 2014, Prof. Dr. Ednaldo Brigante Pizzolato
    Autor: Leonardo de Andrade Cristov�o
    Bibliografia: Primeiro Curso de programa��o em C, Edson Luiz Fran�a Senne.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Cria��o da estrutura cliente
typedef struct
{
    int nconta;          //N�mero da conta
    char nome[20];       //Nome do cliente
    float limite;        //Limite  em reais para efetuar saques
    float saldo;         //Saldo da conta corrente
    float saldoPoupanca; //Saldo da conta poupan�a
    char senha[5];       //Senha da conta do cliente
} cliente;
//Cria��o da estrutura gerente
typedef struct
{
    char senha[5];       //Senha do gerente que permite cria��o de novas contas
} gerente;

FILE *arq;
cliente cli;
gerente ger;
//Fun��o Menu para selecionar opera��es por meio de um caracter
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
    return (toupper(getche())); //Captura o char no instante que ele � digitado(sem usar Enter), torna-o mai�sculo(caso n�o seja)
}
//Fun��o que acessa sequencialmente os registros do tipo cliente para encontrar o n�mero de uma conta
//Essa fun��o presume que o ponteiro esteja inicialmente apontando para a primeira struct cliente no arquivo
int encontra_conta(int n)
{
    fread(&cli,sizeof(cliente),1,arq); //A cada fread, o ponteiro avan�a para o registro seguinte
    while(!feof(arq))
    {
        if(cli.nconta == n)
        {
            fseek(arq, -sizeof(cliente), SEEK_CUR); //Posiciona o ponteiro no in�cio do registro que contenha o numero da conta desejada
            return 1; //Retorna 1 caso a conta seja encontrada
        }
        fread(&cli, sizeof(cliente), 1, arq);
    }
    return 0; //Retorna 0 caso a conta n�o seja encontrada
}
//Procedimento que captura uma senha de 4 digitos e imprime na tela asteriscos conforme se digita, e retorna essa senha por par�metro
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
//Procedimento que recebe por par�metro uma senha a ser criptografada, retornando-a pelo segundo par�metro
void cripto_senha(char *senha, char *novasenha)
{
    char *p, *pnova;
    int i=0;

    p = senha;
    pnova = novasenha;
    while(*p != '\0')
    {
        *pnova = *p+i;  //M�todo de criptografa��o
        pnova++;
        p++;
        i++;
    }
    *pnova = '\0';      //Coloca o indicador de final de frase na senha criptografada
}
//Procedimento que recebe por par�metro uma senha a ser descriptografada, retornando-a pelo segundo par�metro
void descripto_senha(char *senha, char *novasenha)
{
    char *p, *pnova;
    int i=0;

    p = senha;
    pnova = novasenha;
    while(*p != '\0')
    {
        *pnova = *p-i;  //M�todo de descriptografa��o
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

    arq = fopen("banco.dat", "r+b"); //Abre o arquivo bin�rio existente para leitura e grava��o de dados
    if(arq == NULL)
    {
        printf("\nErro ao abrir o arquivo\n");
        return 1;
    }
    printf("\n");
    printf("Numero da conta ..... ");//N�mero da conta que se deseja mudar a senha
    scanf("%d", &num);
    if(encontra_conta(num) == 0)     //Verifica se a conta existe
        printf("\nEssa conta nao existe!\n");
    else
    {
        fread(&cli, sizeof(cliente), 1, arq);
        obtem_senha(senha);                          //Chamada do procedimento para capturar a senha que deseja ser mudada
        descripto_senha(cli.senha, novasenha);       //Chamada do procedimento para descriptograf�-la
        if(!strcmp(novasenha, senha))                //Compara se a senha digitada e a senha descriptografada s�o iguais
        {
            obtem_senha(mudasenha);                  //Chamada do procedimento para capturar a nova senha desejada
            cripto_senha(mudasenha, novamudasenha);  //Chamada do procedimento para criptograf�-la
            strcpy(cli.senha, novamudasenha);        //Copia a senha criptografada na senha do cliente
            fseek(arq, -sizeof(cliente), SEEK_CUR);  //Posiciona o ponteiro no in�cio do registro desejado
            fwrite(&cli, sizeof(cliente), 1, arq);   //Escreve no arquivo bin�rio a modifica��o
            printf("Senha alterada.\n");
        }
        else
            printf("Senha incorreta.");
    }
    fclose(arq);
}
//Procedimento exclusivo para privil�gio do gerente que permite a cria��o de novas contas
void abertura_conta()
{
    int num;
    char senha[5], novasenha[5], senhaGerente[5], novasenhaGerente[5];

    arq = fopen("banco.dat", "r+b");     //Abre o arquivo bin�rio existente para leitura e grava��o de dados
    if(arq == NULL)                      //Se o arquivo n�o existir, ent�o o gerente deve assumir controle para cria��o da primeira conta
    {
        //Criacao da senha do gerente
        arq = fopen("banco.dat", "a+b"); //Abre o arquivo bin�rio para anexa��o de dados, se o arquivo n�o existir, ser� criado um novo arquivo
        if(arq == NULL)
            exit(1);
        printf("\nPrimeiro acesso. \nCrie a senha do gerente:\n");
        obtem_senha(senhaGerente);                    //Captura para criar a senha do gerente
        cripto_senha(senhaGerente, novasenhaGerente); //Criptografa a senha digitada e retorna-a por par�metro
        strcpy(ger.senha, novasenhaGerente);          //Copia a senha criptografada na vari�vel senha do gerente
        fwrite(&ger, sizeof(gerente), 1, arq);        //Escreve no arquivo a senha criptografada
        printf("Senha criada.\n");
    }
    printf("\n");                                     //Assume-se que se chegou nesse ponto, j� existe um arquivo e o gerente j� possui uma senha
    printf("Digite a senha do gerente:\n");           //Pedindo a senha do gerente para que ele possa criar uma nova conta
    obtem_senha(senhaGerente);                        //Captura a senha digitada
    fread(&ger, sizeof(gerente), 1, arq);             //Le no arquivo a estrura gerente
    descripto_senha(ger.senha, novasenhaGerente);     //Procedimento que descriptografa a senha lida no arquivo bin�rio e retorna-a por par�metro
    if(!strcmp(senhaGerente, novasenhaGerente))       //Compara se a senha digitada � a mesma da senha descriptografaca
    {
        arq = fopen("banco.dat", "r+b");              //Abre o arquivo bin�rio existente para leitura e grava��o de dados
        fseek(arq, sizeof(cliente), SEEK_SET);        //Posiciona o ponteiro para o �nicio da primeira estrutra cliente no arquivo bin�rio
        printf("Escolha o numero da conta: ");
        scanf("%d", &num);
        if (encontra_conta(num) == 1)                 //Verifica se a conta j� existe
            printf("\nEssa conta ja existe!\n");
        else
        {
            cli.nconta = num;
            printf("Digite os seguintes dados:\n");
            printf("  Nome do cliente ..... ");
            fflush(stdin);
            gets(cli.nome);                           //Captura o nome do cliente at� que seja pressionada a tecla enter
            printf("  ");
            obtem_senha(senha);
            cripto_senha(senha, novasenha);
            strcpy(cli.senha, novasenha);
            printf("  Limite de credito ... ");
            scanf("%f", &cli.limite);
            printf("  Saldo inicial ....... ");
            scanf("%f", &cli.saldo);
            fwrite(&cli, sizeof(cliente), 1, arq);    //Escreve no arquivo bin�rio a nova conta do cliente
            printf("Conta %d aberta.\n", num);
        }
    }
    else
        printf("Senha incorreta.");
    fclose(arq);
}
//Procedimento que faz a leitura do arquivo bin�rio para mostrar na tela o saldo de uma conta
void listagem_saldo()
{
    int num;
    char senha[5], novasenha[5];

    arq = fopen("banco.dat","r+b"); //Abre o arquivo bin�rio existente para leitura e grava��o de dados
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
//Fun��o que escolhe entre conta Corrente ou conta Poupan�a
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
    return tipoConta;    //Retorna um char C para conta Corrente ou P para conta Poupan�a
}
//Procedimento para realiza��o de dep�sito em conta corrente ou conta poupan�a
void deposito()
{
    int num;
    float valor;
    char tipoConta;
    char senha[5], novasenha[5];

    arq = fopen("banco.dat", "r+b"); //Abre o arquivo bin�rio existente para leitura e grava��o de dados
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
        descripto_senha(cli.senha, novasenha);  //Descriptografa a senha que est� escrita no arquivo
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
                tipoConta = escolheConta();     //Chamada da fun��o para escolher se o dep�sito ser� feito na conta corrente ou conta poupan�a
                if(tipoConta == 'C')
                {
                    cli.saldo = cli.saldo + valor;                 //Deposita na conta corrente
                    printf("\nDeposito efetuado.\n");
                }
                else
                {
                    cli.saldoPoupanca = cli.saldoPoupanca + valor; //Deposita na conta poupan�a
                    printf("\nDeposito efetuado.\n");
                }
                fseek(arq, -sizeof(cliente), SEEK_CUR);            //Posiciona o ponteiro no in�cio do registro desejado
                fwrite(&cli, sizeof(cliente), 1, arq);             //Escreve no arquivo bin�rio a modifica��o
            }
        }
        else
            printf("Senha incorreta.");
    }
    fclose(arq);
}
//Procedimento para realizar saque em conta corrente ou conta poupan�a
void saque()
{
    int num;
    float valor;
    char tipoConta;
    char senha[5], novasenha[5];

    arq = fopen("banco.dat", "r+b"); //Abre o arquivo bin�rio existente para leitura e grava��o de dados
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
                tipoConta = escolheConta();    //Escolhe entre conta corrente ou conta poupan�a
                if(tipoConta == 'C')
                    if(cli.saldo + cli.limite >= valor) //Verifica se � poss�vel sacar aquele valor
                    {
                        cli.saldo = cli.saldo - valor;
                        printf("\nSaque efetudado.\n");
                    }
                    else
                        printf("\nSaque excede o limite (MAX = %.2f)\n", cli.saldo+cli.limite); //Mostra qual a quantidade m�xima que pode ser sacada
                else
                {
                    if(cli.saldoPoupanca + cli.limite >= valor)
                    {
                        cli.saldoPoupanca = cli.saldoPoupanca - valor;
                        printf("\nSaque efetuado.\n");
                    }
                    else
                        printf("\nSaque excede o limite (MAX = %.2f)\n", cli.saldo+cli.limite); //Mostra qual a quantidade m�xima que pode ser sacada
                }
                fseek(arq, -sizeof(cliente), SEEK_CUR); //Posiciona o ponteiro no in�cio da estrutura desejada
                fwrite(&cli, sizeof(cliente), 1, arq);  //Escreve no arquivo bin�rio a estrutura modificada acima
            }
        }
        else
            printf("Senha incorreta.");
    }
    fclose(arq);
}
//Procedimento para transfer�ncia entre conta corrente e conta poupan�a de um determinado cliente
void transferencia()
{
    int num;
    float valor;
    char tipoTransferencia;
    char senha[5], novasenha[5];

    arq = fopen("banco.dat", "r+b");  //Abre o arquivo bin�rio existente para leitura e grava��o de dados
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
        fread(&cli, sizeof(cliente), 1, arq);   //L� a estrutura cliente
        obtem_senha(senha);                     //Captura a senha digitada
        descripto_senha(cli.senha, novasenha);  //Descriptografa a senha do cliente que est� escrita no arquivo
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
            printf(" (A)De poupanca para corrente\n"); //Digita a para transfer�ncia de conta poupan�a para corrente
            printf(" (B)De corrente para poupanca\n"); //Digita b para transfer�ncia de conta corrente para poupan�a
            printf("> ");
            tipoTransferencia = toupper(getche());     //Captura sem a necessidade de pressionar Enter
            while(tipoTransferencia != 'A' && tipoTransferencia != 'B')
            {
                printf("\nEntre apenas com A ou B: ");
                tipoTransferencia = toupper(getche());
            }
            if(tipoTransferencia == 'A')
            {
                cli.saldoPoupanca = cli.saldoPoupanca - valor;  //Retira o valor da conta Poupan�a
                cli.saldo = cli.saldo + valor;                  //Coloca o valor na conta Corrente
                printf("\nTransferencia efetuada.\n");
            }
            else
            {
                cli.saldo = cli.saldo - valor;                 //Retira o valor da conta corrente
                cli.saldoPoupanca = cli.saldoPoupanca + valor; //Coloca o valor na conta poupan�a
                printf("\nTransferencia efetuada.\n");
            }
            fseek(arq, -sizeof(cliente), SEEK_CUR); //Posiciona o ponteiro no in�cio da estrutura a ser modificada
            fwrite(&cli, sizeof(cliente), 1, arq);  //Escreve no arquivo as modifica��es feitas na opera��o acima
        }
        else
            printf("Senha incorreta.");
    }
    fclose(arq);
}
//Procedimento da retirada de um valor da conta corrente que ser� transferido para outra conta
void saque_para_transferencia(int *num, float *valor, int *limite)
{
    char senha[5], novasenha[5];
    char tipoTransferencia;

    arq = fopen("banco.dat", "r+b"); //Abre o arquivo bin�rio existente para leitura e grava��o de dados
    if(arq == NULL)
    {
        printf("\nErro ao abrir o arquivo\n");
        return 1;
    }
    printf("\n");
    printf("Numero da conta ..... ");
    scanf("%d", num);                 //Escolhe a conta na qual ser� retirado o dinheiro a ser transferido
    if(encontra_conta(*num) == 0)
        printf("\nEssa conta nao existe!\n");
    else
    {
        fread(&cli, sizeof(cliente), 1, arq);  //L� a estrutura do cliente que tranfirir�
        obtem_senha(senha);                    //Captura a senha digitada
        descripto_senha(cli.senha, novasenha); //Descriptografa a senha do cliente, gravado no arquivo bin�rio
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
                if(*valor < 1000) //Condi��o para ser transferencia do tipo DOC
                    printf("A transferencia sera do tipo DOC.");
                else
                    if(*valor < 5000) //Condi��o que permite escolha entre transfer�ncia do tipo DOC ou tipo TED
                    {
                        printf("Tipo de transferencia:\n");
                        printf(" (D)OC\n");
                        printf(" (T)ED\n");
                        printf("> ");
                        tipoTransferencia = toupper(getche());  //Captura sem a necessidade de Enter a op��o desejada
                        if(tipoTransferencia == 'D')
                            printf("A transferencia sera do tipo DOC.");
                        else
                            if(tipoTransferencia == 'C')
                                printf("A transferencia sera do tipo TED");
                    }
                    else
                        printf("A transferencia sera do tipo TED."); //Condi��o para transferencia ser do tipo TED
                if(cli.saldo + cli.limite >= *valor)  //Verifica se � poss�vel fazer o saque para transfer�ncia
                    cli.saldo = cli.saldo - *valor;
                else
                {
                    printf("\nSaque excede o limite (MAX = %.2f)\n", cli.saldo+cli.limite);  //Mostra na tela o limite m�ximo que pode ser sacado
                    *limite = 1;  //Retorna por par�metro caso o limite seja ultrapassado
                }
                fseek(arq, -sizeof(cliente), SEEK_CUR);  //Posiciona o ponteiro no in�cio da estrutura cliente
                fwrite(&cli, sizeof(cliente), 1, arq);   //Escreve as modifica��es realizadas acima na estrutura cliente
            }
        }
        else
            printf("Senha incorreta.");
    }
    fclose(arq);
}
//Procedimento de dep�sito de um valor vindo de uma transferencia na conta corrente ou poupan�a
void deposito_para_transferencia(float valor, int *retorno)
{
    int num;
    char tipoConta;

    arq = fopen("banco.dat", "r+b"); //Abre o arquivo bin�rio existente para leitura e grava��o de dados
    if(arq == NULL)
        return 1;

    printf("\n");
    printf("Conta destino ....... ");
    scanf("%d", &num);                 //Escolhe a conta na qual ser� depositado a transferencia
    if(encontra_conta(num) == 0)
    {
        printf("\nEssa conta nao existe!\n");
        *retorno = 1;  //Retorna por par�metro caso a conta destino n�o exista
    }
    else
    {
        fread(&cli, sizeof(cliente), 1, arq);  //L� a estrutura do cliente destino
        tipoConta = escolheConta();  //Escolhe entre conta corrente ou conta poupan�a
        if(tipoConta == 'C')
        {
            cli.saldo = cli.saldo + valor; //Deposita na conta corrente
            printf("\nTransferencia efetuada.\n");
        }
        else
        {
            cli.saldoPoupanca = cli.saldoPoupanca + valor;  //Deposita na conta poupan�a
            printf("\nTransferencia efetuada.\n");
        }
        fseek(arq, -sizeof(cliente), SEEK_CUR);  //Posiciona o ponteiro no in�cio da estrutura a ser modificada
        fwrite(&cli, sizeof(cliente), 1, arq);   //Escreve no arquivo bin�rio as altera��es feitas na conta destino
    }
    fclose(arq);
}
//Procedimento para caso o limite a ser transferido � ultrapassado, ou caso a conta destino n�o existir
void deposita_de_volta(int num, float valor)
{
    arq = fopen("banco.dat", "r+b"); //Abre o arquivo bin�rio existente para leitura e grava��o de dados
    if(arq == NULL)
    {
        printf("\nErro ao abrir o arquivo\n");
        return 1;
    }
    if(encontra_conta(num) == 0)
        printf("\nEssa conta nao existe!\n");
    else
    {
        fread(&cli, sizeof(cliente), 1, arq);   //L� a estrutura do cliente origem
        cli.saldo = cli.saldo + valor;          //Deposita o dinheiro que n�o pode ser transferido
        printf("\nTransferencia retornada.\n");
        fseek(arq, -sizeof(cliente), SEEK_CUR); //Posiciona o ponteiro no in�cio da estrutura do cliente a ser modificado
        fwrite(&cli, sizeof(cliente), 1, arq);  //Escreve no arquivo bin�rio as altera��es feitas no procedimento
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

    //Retorna por par�metro o numero da conta que transfere, o valor transferido e se o limite foi atingido ou n�o.
    saque_para_transferencia(&contaOrigem, &valorTransferencia, &limite);
    if(limite == 0) //Se o limite n�o foi atingido, ent�o executa-se o procedimento para depositar o valor na conta destino
        deposito_para_transferencia(valorTransferencia, &retorno); //Retorna se a conta destino existe ou n�o
    else
        printf("Transferencia nao efetuada.");
    if(retorno == 1) //Se a conta n�o existir, ent�o executa-se o procedimento para retornar o valor na conta origem
        deposita_de_volta(contaOrigem, valorTransferencia);
}


int main(int args, char * arg[])
{
    char op;

    do
    {
        //Ferramenta para escolher as opc�es do menu
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
    //Executa-se pelo menos uma vez a escolha das opera��es at� que seja pressionado f para finalizar.

    return 0;
}
