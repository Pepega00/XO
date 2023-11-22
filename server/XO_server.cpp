 // inet_addr
#include <arpa/inet.h>
 
#include <cstring>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <random>
#include <chrono>
#include <ctime>

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

void write_log(std::string message){
    std::ofstream logfile("log.txt",std::ios::app);
    time_t time=std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    logfile<<ctime(&time)<<'\t'<<message<<'\n';
    logfile.close();
} 

void clear_buffer(char* buffer){
    for(int i=0;i<1024;++i){
        buffer[i]=0;
    }
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
            write_log("player X won");
            return true;
        }
        else{
            std::cout<<"player O won\n";
            write_log("player O won");
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
            write_log("player X won");
            return true;
        }
        else{
            std::cout<<"player O won\n";
            write_log("player O won");
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
                write_log("player X won");
                return true;
            }
            else{
                std::cout<<"player O won\n";
                write_log("player O won");
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
                write_log("player X won");
                return true;
            }
            else{
                std::cout<<"player O won\n";
                write_log("player O won");
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
        write_log("game ended with draw");
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
    game_finished=check_victory(board,cell_number);
}
void read_config(int& port,int& time_limit){
    std::ifstream config_file("serverconfig.txt");
    std::string tmp_line;
    getline(config_file,tmp_line);
    for(int i=7;i<tmp_line.length();++i){
        port+=tmp_line[i]-48;
        if(i<tmp_line.length()-1){
            port*=10;
        }
    }
    getline(config_file,tmp_line);
    for(int i=10;i<tmp_line.length();++i){
        time_limit+=tmp_line[i]-48;
        if(i<tmp_line.length()-1){
            time_limit*=10;
        }
    }
}
void read_database(std::string nicknames[],std::string passwords[],int& db_counter){
    std::ifstream database("database.txt");
    while(database.peek()!=EOF){
        char line [50]={0};
        database.getline(line,50);
        int counter=0;
        while(line[counter]!=' '){
            nicknames[db_counter]+=line[counter];
            ++counter;
        }
        ++counter;
        while(line[counter]!=0 && counter<50){
            passwords[db_counter]+=line[counter];
            ++counter;
        }
        ++db_counter;
    }
    database.close();
}
void write_buffer(char* buffer){
    for(int i=0;buffer[i]!=0 && i<1024;++i){
        std::cout<<buffer[i];
    }
    std::cout<<'\n';
}
void registration(int X_Socket,int O_Socket){
    int buf_size=1024;
    char buffer[1024]={0};
    std::string nicknames[50];
    std::string passwords[50];
    bool is_online[50]={0};
    int db_counter=0;
    read_database(nicknames,passwords,db_counter);
    bool X_signing_in=false;
    bool O_signing_in=false;
    int X_progress=0;
    int O_progress=0;
    //0 : choosing between signing in and logging in
    //1 : writing username
    //2 : writing password
    //3 : successful registration
    int X_account_num=-1;
    int O_account_num=-1;
    std::string X_nickname;
    std::string O_nickname;
    while(X_progress<3 || O_progress<3){
        fd_set s;
        struct timeval timeout;
        FD_ZERO(&s);
        FD_SET(X_Socket, &s);
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        int tmp_num=select(X_Socket+1, &s, NULL,NULL, &timeout);
        if(tmp_num==1 && X_progress<3){
            if(X_progress==0){
                clear_buffer(buffer);
                recv(X_Socket,&buffer, buf_size, 0);
                if(strcmp(buffer,"log_in")==0){
                    X_signing_in=false;
                    ++X_progress;
                    send(X_Socket,"correct command",15, 0);
                }
                if(strcmp(buffer,"sign_in")==0){
                    X_signing_in=true;
                    ++X_progress;
                    send(X_Socket,"correct command",15, 0);
                }
                if(X_progress==0){
                    send(X_Socket,"wrong command",13, 0);
                }
            }
            else{
                if(X_signing_in){
                    if(X_progress==1){
                        clear_buffer(buffer);
                        recv(X_Socket,&buffer, buf_size, 0);
                        X_account_num=-1;
                        for(int i=0;i<db_counter;++i){
                            if(strcmp(buffer,nicknames[i].c_str())==0){
                                X_account_num=i;
                            }
                        }
                        if(X_account_num<0 && strcmp(buffer,"exit")!=0){
                            send(X_Socket,"valid username",14, 0);
                            ++X_progress;
                            X_nickname=buffer;
                        }
                        if(X_account_num>=0){
                            send(X_Socket,"username occupied",17, 0);
                            X_progress=0;
                        }
                        if(strcmp(buffer,"exit")==0){
                            send(X_Socket,"exiting",7, 0);
                            X_progress=0;
                        }
                    }
                    else{
                        if(X_progress==2){
                            clear_buffer(buffer);
                            recv(X_Socket,&buffer, buf_size, 0);
                            int pass_num=-1;
                            for(int i=0;i<db_counter;++i){
                                if(strcmp(buffer,passwords[i].c_str())==0){
                                    pass_num=i;
                                }
                            }
                            X_account_num=-1;
                            for(int i=0;i<db_counter;++i){
                                if(X_nickname==nicknames[i]){
                                    X_account_num=i;
                                }
                            }
                            if(pass_num<0 && strcmp(buffer,"exit")!=0  &&  X_account_num<0){
                                send(X_Socket,"X signed in",11, 0);
                                ++X_progress;
                                std::ofstream o_database("database.txt",std::ios::app);
                                if(db_counter>0){
                                    o_database<<'\n';
                                }
                                o_database<<X_nickname<<' '<<buffer;
                                o_database.close();
                                nicknames[db_counter]=X_nickname;
                                passwords[db_counter]=buffer;
                                is_online[db_counter]=true;
                                ++db_counter;
                                write_log("player X signed in");
                            }
                            if(pass_num>=0){
                                send(X_Socket,"password occupied",17, 0);
                                X_progress=0;
                            }
                            if(strcmp(buffer,"exit")==0){
                                send(X_Socket,"exiting",7, 0);
                                X_progress=0;
                            }
                            if(X_account_num>=0){
                                send(X_Socket,"name got taken",14, 0);
                                X_progress=0;
                            }
                        }
                    }
                }
                else{
                    if(X_progress==1){
                        clear_buffer(buffer);
                        recv(X_Socket,&buffer, buf_size, 0);
                        X_account_num=-1;
                        for(int i=0;i<db_counter;++i){
                            if(strcmp(buffer,nicknames[i].c_str())==0){
                                X_account_num=i;
                            }
                        }
                        if(X_account_num>=0 && !is_online[X_account_num]){
                            ++X_progress;
                            is_online[X_account_num]=true;
                            send(X_Socket,"username found",14, 0);
                        }
                        else{
                            if(X_account_num<0){
                                send(X_Socket,"username not found",18, 0);
                                X_progress=0;
                            }
                            if(is_online[X_account_num]){
                                send(X_Socket,"user is online",14, 0);
                                X_progress=0;
                            }
                        }
                    }
                    else{
                        if(X_progress==2){
                            clear_buffer(buffer);
                            recv(X_Socket,&buffer, buf_size, 0);
                            if(strcmp(buffer,passwords[X_account_num].c_str())==0){
                                ++X_progress;
                                send(X_Socket,"X logged in",11, 0);
                                write_log("player X logged in");
                            }
                            else{
                                X_progress=0;
                                send(X_Socket,"wrong password",14, 0);
                            }
                        }
                    }
                }
            }
        }
/////////////////  O side
        FD_ZERO(&s);
        FD_SET(O_Socket, &s);
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        tmp_num=select(O_Socket+1, &s, NULL,NULL, &timeout);
        if(tmp_num==1 && O_progress<3){
            if(O_progress==0){
                clear_buffer(buffer);
                recv(O_Socket,&buffer, buf_size, 0);
                if(strcmp(buffer,"log_in")==0){
                    O_signing_in=false;
                    ++O_progress;
                    send(O_Socket,"correct command",15, 0);
                }
                if(strcmp(buffer,"sign_in")==0){
                    O_signing_in=true;
                    ++O_progress;
                    send(O_Socket,"correct command",15, 0);
                }
                if(O_progress==0){
                    send(O_Socket,"wrong command",13, 0);
                }
            }
            else{
                if(O_signing_in){
                    if(O_progress==1){
                        clear_buffer(buffer);
                        recv(O_Socket,&buffer, buf_size, 0);
                        O_account_num=-1;
                        for(int i=0;i<db_counter;++i){
                            if(strcmp(buffer,nicknames[i].c_str())==0){
                                O_account_num=i;
                            }
                        }
                        if(O_account_num<0 && strcmp(buffer,"exit")!=0){
                            send(O_Socket,"valid username",14, 0);
                            ++O_progress;
                            O_nickname=buffer;
                        }
                        if(O_account_num>=0){
                            send(O_Socket,"username occupied",17, 0);
                            O_progress=0;
                        }
                        if(strcmp(buffer,"exit")==0){
                            send(O_Socket,"exiting",7, 0);
                            O_progress=0;
                        }
                    }
                    else{
                        if(O_progress==2){
                            clear_buffer(buffer);
                            recv(O_Socket,&buffer, buf_size, 0);
                            int pass_num=-1;
                            for(int i=0;i<db_counter;++i){
                                if(strcmp(buffer,passwords[i].c_str())==0){
                                    pass_num=i;
                                }
                            }
                            O_account_num=-1;
                            for(int i=0;i<db_counter;++i){
                                if(O_nickname==nicknames[i]){
                                    O_account_num=i;
                                }
                            }
                            if(pass_num<0 && strcmp(buffer,"exit")!=0 && O_account_num<0){
                                send(O_Socket,"O signed in",11, 0);
                                ++O_progress;
                                std::ofstream o_database("database.txt",std::ios::app);
                                if(db_counter>0){
                                    o_database<<'\n';
                                }
                                o_database<<O_nickname<<' '<<buffer;
                                o_database.close();
                                nicknames[db_counter]=O_nickname;
                                passwords[db_counter]=buffer;
                                is_online[db_counter]=true;
                                ++db_counter;
                                write_log("player O signed in");
                            }
                            if(pass_num>=0){
                                send(O_Socket,"password occupied",17, 0);
                                O_progress=0;
                            }
                            if(strcmp(buffer,"exit")==0){
                                send(O_Socket,"exiting",7, 0);
                                O_progress=0;
                            }
                            if(O_account_num>=0){
                                send(O_Socket,"name got taken",14, 0);
                                O_progress=0;
                            }
                        }
                    }
                }
                else{
                    if(O_progress==1){
                        clear_buffer(buffer);
                        recv(O_Socket,&buffer, buf_size, 0);
                        O_account_num=-1;
                        for(int i=0;i<db_counter;++i){
                            if(strcmp(buffer,nicknames[i].c_str())==0){
                                O_account_num=i;
                            }
                        }
                        if(O_account_num>=0 && !is_online[O_account_num]){
                            ++O_progress;
                            is_online[O_account_num]=true;
                            send(O_Socket,"username found",14, 0);
                        }
                        else{
                            if(O_account_num<0){
                                send(O_Socket,"username not found",18, 0);
                                O_progress=0;
                            }
                            if(is_online[O_account_num]){
                                send(O_Socket,"user is online",14, 0);
                                O_progress=0;
                            }
                        }
                    }
                    else{
                        if(O_progress==2){
                            clear_buffer(buffer);
                            recv(O_Socket,&buffer, buf_size, 0);
                            if(strcmp(buffer,passwords[O_account_num].c_str())==0){
                                ++O_progress;
                                send(O_Socket,"O logged in",11, 0);
                                write_log("player O logged in");
                            }
                            else{
                                O_progress=0;
                                send(O_Socket,"wrong password",14, 0);
                            }
                        }
                    }
                }
            }
        }
    }
}
int main()
{   
    int port=0;
    int time_limit=0;
    read_config(port,time_limit);
    // Initialize variables
    int serverSocket, newSocket;
    int X_Socket;
    int O_Socket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
 
    socklen_t addr_size;
 
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
 
    // Bind the socket to the
    // address and port number.
    bind(serverSocket,
         (struct sockaddr*)&serverAddr,
         sizeof(serverAddr));
 
    if (listen(serverSocket, 0) == 0){
        printf("Listening\n");
    } 
    else{
        printf("Error\n");
    }
    write_log("server launched");
    addr_size = sizeof(serverStorage);
    X_Socket = accept(serverSocket,(struct sockaddr*)&serverStorage,&addr_size);
    write_log("player 1 connected");
    addr_size = sizeof(serverStorage);
    O_Socket = accept(serverSocket,(struct sockaddr*)&serverStorage,&addr_size);
    write_log("player 2 connected");
    bool b=std::rand()%2;
    if(b){
        int tmp=X_Socket;
        X_Socket=O_Socket;
        O_Socket=tmp;
        write_log("player 1 is O,player 2 is X");
    }
    else{
        write_log("player 1 is X,player 2 is O");
    }
    registration(X_Socket,O_Socket);
    write_log("starting the game");
    char buffer[1024]={0};
    bool game_finished=false;
    cell** board=new cell*[3];
    for (int i=0;i<3;++i){
        board[i]=new cell[3];
    }
    while(!game_finished){
        fd_set s;
        struct timeval timeout;
        FD_ZERO(&s);
        FD_SET(X_Socket, &s);
        timeout.tv_sec = time_limit;
        timeout.tv_usec = 0;
        int tmp_num=select(X_Socket+1, &s, NULL,NULL, &timeout);
        if(tmp_num==1){
            clear_buffer(buffer);
            recv(X_Socket,&buffer, sizeof(buffer), 0);
            std::string log_msg="player X made a turn (cell ";
            log_msg+=buffer[0];
            log_msg+=")";
            write_log(log_msg);
            send(X_Socket, "no time out",11, 0);
            fill_cell(board,buffer[0]-48,true,game_finished);
            send(O_Socket, buffer,strlen(buffer), 0);
        }
        else{
            std::cout<<"O player won on time\n";
            send(O_Socket, "X timed out",11, 0);
            send(X_Socket, "X timed out",11, 0);
            write_log("player X timed out");
            game_finished=true;
        }
        if(!game_finished){
            FD_ZERO(&s);
            FD_SET(O_Socket, &s);
            timeout.tv_sec = time_limit;
            timeout.tv_usec = 0;
            tmp_num=select(O_Socket+1, &s, NULL,NULL, &timeout);
            if(tmp_num==1){
                clear_buffer(buffer);
                recv(O_Socket,&buffer, sizeof(buffer), 0);
                std::string log_msg="player O made a turn (cell ";
                log_msg+=buffer[0];
                log_msg+=")";
                write_log(log_msg);
                send(O_Socket, "no time out",11, 0);
                fill_cell(board,buffer[0]-48,false,game_finished);
                send(X_Socket, buffer,strlen(buffer), 0);
            }
            else{
                std::cout<<"X player won on time\n";
                send(X_Socket, "O timed out",11, 0);
                send(O_Socket, "O timed out",11, 0);
                write_log("player O timed out");
                game_finished=true;
            }
        }
    }
    write_log("game finished");

    for(int i=0;i<3;++i){
        delete[] board[i];
    }
    delete[] board;
    write_log("closing the server");
    close(serverSocket);
    return 0;
}