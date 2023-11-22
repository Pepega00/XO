// C program for the Client Side
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
 
// inet_addr
#include <arpa/inet.h>
#include <unistd.h>
 
// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>

#include <iostream>
#include <string>
#include <fstream>
class cell
{
private:
    bool empty=true;
    bool X=false;
public:
    cell();
    void set_empty(bool X){
        empty=X;
    }
    void set_X(bool x){
        X=x;
    }
    bool get_empty(){
        return empty;
    }
    bool get_X(){
        return X;
    }
    ~cell();
};

cell::cell()
{
}

cell::~cell()
{
}

void clear_buffer(char* buffer){
    for(int i=0;i<1024;++i){
        buffer[i]=0;
    }
}
void write_buffer(char* buffer){
    for(int i=0;buffer[i]!=0 && i<1024;++i){
        std::cout<<buffer[i];
    }
    std::cout<<'\n';
}
bool check_victory(cell** board,int cell_number){
    int row=(cell_number-1)/3;
    int col=(cell_number-1)%3;
    //check the row
    if(
        !board[row][0].get_empty() && 
        !board[row][1].get_empty() && 
        !board[row][2].get_empty() && 
        board[row][0].get_X()==board[row][1].get_X() && 
        board[row][1].get_X()==board[row][2].get_X()
    ){
        if(board[row][0].get_X()){
            std::cout<<"player X won\n";
            return true;
        }
        else{
            std::cout<<"player O won\n";
            return true;
        }
    }
    //check the col
    if(
        !board[0][col].get_empty() && 
        !board[1][col].get_empty() && 
        !board[2][col].get_empty() && 
        board[0][col].get_X()==board[1][col].get_X() && 
        board[1][col].get_X()==board[2][col].get_X()
    ){
        if(board[0][col].get_X()){
            std::cout<<"player X won\n";
            return true;
        }
        else{
            std::cout<<"player O won\n";
            return true;
        }
    }
    //check diag
    if(row==col){
        if(
            !board[0][0].get_empty() && 
            !board[1][1].get_empty() && 
            !board[2][2].get_empty() && 
            board[0][0].get_X()==board[1][1].get_X() && 
            board[1][1].get_X()==board[2][2].get_X()
        ){
            if(board[1][1].get_X()){
                std::cout<<"player X won\n";
                return true;
            }
            else{
                std::cout<<"player O won\n";
                return true;
            }
        }
    }
    if((row+col)==2){
        if(
            !board[0][2].get_empty() && 
            !board[1][1].get_empty() && 
            !board[2][0].get_empty() && 
            board[0][2].get_X()==board[1][1].get_X() && 
            board[1][1].get_X()==board[2][0].get_X()
        ){
            if(board[1][1].get_X()){
                std::cout<<"player X won\n";
                return true;
            }
            else{
                std::cout<<"player O won\n";
                return true;
            }
        }      
    }
    bool fullboard=true;
    for(int i=0;i<3;++i){
        for(int j=0;j<3;++j){
            if(board[i][j].get_empty()){
                fullboard=false;
            }
        }
    }
    if(fullboard){
        std::cout<<"draw\n";
        return true;
    }
    return false;
}

void show_board(cell** board){
    for(int i=0;i<3;++i){
        for(int j=0;j<3;++j){
            if(board[i][j].get_empty()){
                std::cout<<"*";
            }
            else{
                if(board[i][j].get_X()){
                    std::cout<<"X";
                }
                else{
                    std::cout<<"O";
                }
            }
            std::cout<<" ";
        }
        std::cout<<'\n';
    }
}

void fill_cell(cell** board, int cell_number,bool is_X,bool& game_finished){
    int row=(cell_number-1)/3;
    int col=(cell_number-1)%3;
    board[row][col].set_empty(false);
    board[row][col].set_X(is_X);
    show_board(board);
    game_finished=check_victory(board,cell_number);
}

bool check_cell(cell** board, int cell_number){
    int row=(cell_number-1)/3;
    int col=(cell_number-1)%3;
    if(board[row][col].get_empty()){
        return true;
    }
    return false;
}
void registration(int network_socket,bool& X_player){
    char buffer[1024]={0};
    bool logged_in=false;
    while(!logged_in){
        std::cout<<"type log_in or sign_in\n";
        std::string tmp_command;
        std::cin>>tmp_command;
        if(tmp_command=="log_in"){
            send(network_socket, tmp_command.c_str(), tmp_command.length(), 0);
            clear_buffer(buffer);
            recv(network_socket,&buffer, sizeof(buffer), 0);
            write_buffer(buffer);
            if(strcmp(buffer,"correct command")==0){
                std::cout<<"type your username\n";
                std::string tmp_name;
                std::cin>>tmp_name;
                send(network_socket, tmp_name.c_str(), tmp_name.length(), 0);
                clear_buffer(buffer);
                recv(network_socket,&buffer, sizeof(buffer), 0);
                write_buffer(buffer);
                if(strcmp(buffer,"username found")==0){
                    std::cout<<"type your password\n";
                    std::string tmp_password;
                    std::cin>>tmp_password;
                    send(network_socket, tmp_password.c_str(), tmp_password.length(), 0);
                    clear_buffer(buffer);
                    recv(network_socket,&buffer, sizeof(buffer), 0);
                    std::cout<<"writing buffer\n";
                    write_buffer(buffer);
                    if(strcmp(buffer,"X logged in")==0){
                        logged_in=true;
                        X_player=true;
                    }
                    if(strcmp(buffer,"O logged in")==0){
                        logged_in=true;
                        X_player=false;
                    }
                }
            }
            
        }
        if(tmp_command=="sign_in"){
            send(network_socket, tmp_command.c_str(), tmp_command.length(), 0);
            clear_buffer(buffer);
            recv(network_socket,&buffer, sizeof(buffer), 0);
            write_buffer(buffer);
            if(strcmp(buffer,"correct command")==0){
                std::cout<<"type your username\n";
                std::string tmp_name;
                std::cin>>tmp_name;
                send(network_socket, tmp_name.c_str(), tmp_name.length(), 0);
                clear_buffer(buffer);
                recv(network_socket,&buffer, sizeof(buffer), 0);
                write_buffer(buffer);
                if(strcmp(buffer,"valid username")==0){
                    std::cout<<"type your password\n";
                    std::string tmp_password;
                    std::cin>>tmp_password;
                    send(network_socket, tmp_password.c_str(), tmp_password.length(), 0);
                    clear_buffer(buffer);
                    recv(network_socket,&buffer, sizeof(buffer), 0);
                    write_buffer(buffer);
                    if(strcmp(buffer,"X signed in")==0){
                        logged_in=true;
                        X_player=true;
                    }
                    if(strcmp(buffer,"O signed in")==0){
                        logged_in=true;
                        X_player=false;
                    }
                }
            }           
        }
    }
}
// Driver Code
int main()
{
    int port=0;
    std::ifstream config_file("clientconfig.txt");
    std::string tmp_line;
    getline(config_file,tmp_line);
    for(int i=7;i<tmp_line.length();++i){
        port+=tmp_line[i]-48;
        if(i<tmp_line.length()-1){
            port*=10;
        }
    }
    int network_socket;
 
    // Create a stream socket
    network_socket = socket(AF_INET,SOCK_STREAM, 0);
 
    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);
 
    // Initiate a socket connection
    int connection_status = connect(network_socket,(struct sockaddr*)&server_address,sizeof(server_address));
 
    // Check for connection error
    if (connection_status < 0) {
        puts("Error\n");
        return 0;
    }
 
    printf("Connection established\n");
 
    bool X_player=false;
    registration(network_socket,X_player);
    char buffer[1024] = { 0 };
    bool game_finished=false;
    cell** board=new cell*[3];
    for (int i=0;i<3;++i){
        board[i]=new cell[3];
    }
    show_board(board);
    if(X_player){
        while(!game_finished){
            std::string tmp_cell;
            std::cout<<"pick the number of cell\n";
            while(!(tmp_cell.length()==1 && tmp_cell[0]<='9' && tmp_cell[0]>='1')){
                std::cin>>tmp_cell;
                if(!(tmp_cell.length()==1 && tmp_cell[0]<='9' && tmp_cell[0]>='1')){
                    std::cout<<"invalid input\n";
                }
                else{
                    if(!check_cell(board,tmp_cell[0]-48)){
                        std::cout<<"the cell is occupied\n";
                        tmp_cell.clear();
                    }
                }
            }
            if(!game_finished){
                send(network_socket, tmp_cell.c_str(), tmp_cell.length(), 0);
                clear_buffer(buffer);
                recv(network_socket,&buffer, sizeof(buffer), 0);
                if(strcmp(buffer,"X timed out")==0){
                    std::cout<<"O player won on time\n";
                    game_finished=true;
                }
                else{
                    fill_cell(board,tmp_cell[0]-48,true,game_finished);
                }
                if(!game_finished){
                    std::cout<<"enemy turn\n";
                    clear_buffer(buffer);
                    recv(network_socket,&buffer, sizeof(buffer), 0);
                    if(strcmp(buffer,"O timed out")==0){
                        std::cout<<"X player won on time\n";
                        game_finished=true;
                    }
                    else{
                        fill_cell(board,buffer[0]-48,false,game_finished);
                    }
                }
            }
        }
    }
    else{
        while(!game_finished){
            std::cout<<"enemy turn\n";
            clear_buffer(buffer);
            recv(network_socket,&buffer, sizeof(buffer), 0);
            if(strcmp(buffer,"X timed out")==0){
                std::cout<<"O player won on time\n";
                game_finished=true;
            }
            else{
                fill_cell(board,buffer[0]-48,true,game_finished);
            }
            std::string tmp_cell;
            if(!game_finished){
                std::cout<<"pick the number of cell\n";
                while(!(tmp_cell.length()==1 && tmp_cell[0]<='9' && tmp_cell[0]>='1')){
                    std::cin>>tmp_cell;
                    if(!(tmp_cell.length()==1 && tmp_cell[0]<='9' && tmp_cell[0]>='1')){
                        std::cout<<"invalid input\n";
                    }
                    else{
                        if(!check_cell(board,tmp_cell[0]-48)){
                            std::cout<<"the cell is occupied\n";
                            tmp_cell.clear();
                        }
                    }
                }
            }
            if(!game_finished){
                send(network_socket, tmp_cell.c_str(), tmp_cell.length(), 0);
                clear_buffer(buffer);
                recv(network_socket,&buffer, sizeof(buffer), 0);
                if(strcmp(buffer,"O timed out")==0){
                    std::cout<<"X player won on time\n";
                    game_finished=true;
                }
                else{
                    fill_cell(board,tmp_cell[0]-48,false,game_finished);
                }
            }
        }
    }

    for(int i=0;i<3;++i){
        delete[] board[i];
    }
    delete[] board;
    // Close the connection
    close(network_socket);

}