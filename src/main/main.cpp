#include <iostream>
#include <pqxx/pqxx>
#include <vector>
#include <iomanip>
#include <optional>
#include "service/EventService.h"
#include "service/SportService.h"
#include "service/ParticipantsService.h"
#include "service/BetService.h"
#include "service/UserService.h"
#include "entities/EventEntity.h"
#include "entities/SportEntity.h"
#include "entities/ParticipantsEntity.h"
#include "entities/BetEntity.h"
#include "entities/UserEntity.h"
#include "enum/UserRoleEnum.h"
#include "enum/EventStatusEnum.h"
#include "enum/TypeOfBets.h"
#include "infra/db/configure.h"
#include "cli/index.h"
using namespace std;


int main() {
    try {
        pqxx::connection *conn = connectDataBase();
        UserService userServices;
        ParticipantsService participantsService;
        SportService sportService;
        EventService eventServices;
        BetService betServices;
        size_t option;
        optional<UserEntity> user;
        welcome();
        do{
            option = login();
            if(option == 1){
                pair<string, string> credentials = authAccount();
                user = userServices.authUser(conn, credentials.first, credentials.second);
                if(user){
                    break;
                }
            }
            else if(option == 2){
                optional<UserEntity> userData = createAccount();
                if(userData.has_value()) {
                    if(userServices.alreadyExists(conn, userData.value().getEmail())) {
                        altLinesFormat("Email já cadastrado!");
                    }
                }
                if(userData){
                    bool isUserSaved = userServices.save(conn, &(*userData));
                    if(isUserSaved){
                        user = userData;
                        altLinesFormat("Usuário cadastrado com sucesso!");
                        break;
                    }
                }
            }
            else if(option == 0){
                altLinesFormat("Saindo...Sentiremos sua falta;-;");
            }else{
                altLinesFormat("Digite uma opção válida");
            }
        }while(option != 0);


        optional<vector<EventEntity>> events;
        optional<EventEntity> event;
        optional<SportEntity> sport;
        optional<ParticipantsEntity> participant;
        optional<vector<BetEntity>> bets;
        BetEntity *bet;
        size_t eventId, betChoice;
        TypeOfBets betType;
        double amount;

        if(user && user->getRole() == UserRoleEnum::ADMIN){
            do{
                option = adminMenu();
                switch(option){
                    case 1:
                        events = eventServices.findAll(conn);
                        if(events){
                            for (const auto& event : events.value()) {
                                cout << "\n" << endl;
                                tableFormat("ID", 0);
                                tableFormat("Horário", 1);
                                tableFormat("Status", 4);
                                tableFormat("Esporte", 1);
                                tableFormat("Odd vitória time A", 6);
                                tableFormat("Odd vitória time B", 6);
                                tableFormat("Odd empate", 2);
                                event.toString();
                            }
                        }
                        break;
                    case 2: 
                        events = eventServices.findAll(conn);
                        if(events){
                            for (const auto& event : events.value()) {
                                cout << "\n" << endl;
                                tableFormat("ID", 0);
                                tableFormat("Horário", 1);
                                tableFormat("Status", 4);
                                tableFormat("Esporte", 1);
                                tableFormat("Odd vitória time A", 6);
                                tableFormat("Odd vitória time B", 6);
                                tableFormat("Odd empate", 2);
                                event.toString();
                            }
                        }
                        cout << "Digite o id do Evento: ";
                        cin >> eventId;
                        getchar();
                        event = eventServices.findById(conn, eventId);
                        
                        cout << "Digite o valor a ser apostado: ";
                        cin >> amount;
                        getchar();
                        
                        cout << "1) " << event.value().getTeamA().getName() << " vencedor" << endl;
                        cout << "2) " << event.value().getTeamB().getName() << " vencedor" << endl;
                        cout << "3) Empate" << endl;
                        cout << "Escolha uma das opções de aposta: ";
                        cin >> betChoice;
                        getchar();
                        if(betChoice == 1){ betType = TypeOfBets::VITORIA_TIME_A; }
                        if(betChoice == 2){ betType = TypeOfBets::VITORIA_TIME_B; }
                        if(betChoice == 3){ betType = TypeOfBets::EMPATE; }

                        bet = new BetEntity(user.value(), event.value(), amount, betType);
                        try{
                            betServices.save(conn, bet);
                        } catch(const std::exception& e){
                            altLinesFormat("Evento não mais disponível"); 
                            break;
                        } 
                        altLinesFormat("Aposta feita com sucesso!");    
                        break;
                    case 3:
                        bets = betServices.findAll(conn);
                        if(bets){
                            for(const auto& bet : bets.value()){
                                if(bet.getUser().getId() == user.value().getId()){
                                    bet.toString();
                                    cout << "\n";
                                }
                            }
                        }
                        break;
                    case 4:
                        if(user.has_value()) {
                            user = userServices.findById(conn, user.value().getId());
                            switch(account()) {
                            case 1: 
                                userServices.deposit(conn, user.value());
                                break;
                            case 2: 
                                userServices.withdraw(conn, user.value());    
                                break;
                            case 3:
                                linesFormat("CONTA");
                                cout << "R$ " << fixed  << setprecision(2) << user.value().getBalance() << endl;
                                break;
                            case 0: break;
                            default: 
                                altLinesFormat("Digite uma opção válida"); 
                                break;
                            }
                        }
                        break;
                    case 5: 
                        sport = createSport();
                        if(sport.has_value()) {
                            SportEntity *newSport = new SportEntity;
                            *newSport = sport.value();
                            sportService.save(conn, newSport);
                            delete(newSport);
                        }
                        break;
                    case 6: 
                        participant = createParticipant();
                        if(participant.has_value()) {
                            ParticipantsEntity *newParticipant = new ParticipantsEntity;
                            *newParticipant = participant.value();
                            participantsService.save(conn, newParticipant);
                            delete(newParticipant);
                        }
                        break;
                    case 7:
                        event = createNewEvent(conn);
                        if(event.has_value()) {
                            EventEntity *newEvent = new EventEntity;
                            *newEvent = event.value();
                            eventServices.save(conn, newEvent);
                            delete(newEvent);
                        }
                        break;
                    case 8:
                        changeEventStatus(conn);
                        break;
                    case 0:  
                        altLinesFormat("Saindo...Sentiremos sua falta;-;");
                        break;
                    default:
                        altLinesFormat("Digite uma opção válida");
                        break;
                }
            } while(option != 0);
        }
        else if(user->getRole() == UserRoleEnum::USUARIO){
            do{
                option = menu();
                optional<vector<EventEntity>> events;
                optional<EventEntity> event;
                optional<vector<BetEntity>> bets;
                BetEntity *bet;
                size_t eventId, betChoice;
                TypeOfBets betType;
                double amount;
                switch(option){
                    case 1:
                            events = eventServices.findAll(conn);
                            if(events){
                                for (const auto& event : events.value()) {
                                    tableFormat("ID", 0);
                                    tableFormat("Horário", 1);
                                    tableFormat("Status", 4);
                                    tableFormat("Esporte", 1);
                                    tableFormat("Odd vitória time A", 6);
                                    tableFormat("Odd vitória time B", 6);
                                    tableFormat("Odd empate", 2);
                                    event.toString();
                                }
                            }
                        break;
                    case 2: 
                        events = eventServices.findAll(conn);
                        if(events){
                            for (const auto& event : events.value()) {
                                cout << "\n" << endl;
                                tableFormat("ID", 0);
                                tableFormat("Horário", 1);
                                tableFormat("Status", 4);
                                tableFormat("Esporte", 1);
                                tableFormat("Odd vitória time A", 6);
                                tableFormat("Odd vitória time B", 6);
                                tableFormat("Odd empate", 2);
                                event.toString();
                            }
                        }
                        cout << "Digite o id do Evento: ";
                        cin >> eventId;
                        getchar();
                        event = eventServices.findById(conn, eventId);
                        
                        cout << "Digite o valor a ser apostado: ";
                        cin >> amount;
                        getchar();
                        
                        cout << "1) " << event.value().getTeamA().getName() << " vencedor" << endl;
                        cout << "2) " << event.value().getTeamB().getName() << " vencedor" << endl;
                        cout << "3) Empate" << endl;
                        cout << "Escolha uma das opções de aposta: ";
                        cin >> betChoice;
                        getchar();
                        if(betChoice == 1){ betType = TypeOfBets::VITORIA_TIME_A; }
                        if(betChoice == 2){ betType = TypeOfBets::VITORIA_TIME_B; }
                        if(betChoice == 3){ betType = TypeOfBets::EMPATE; }

                        bet = new BetEntity(user.value(), event.value(), amount, betType);
                        try{
                            betServices.save(conn, bet);
                        } catch(const std::exception& e){
                            altLinesFormat("Evento não mais disponível"); 
                            break;
                        } 
                        altLinesFormat("Aposta feita com sucesso!");    
                        break;
                    case 3:
                        bets = betServices.findAll(conn);
                        if(bets){
                            for(const auto& bet : bets.value()){
                                if(bet.getUser().getId() == user.value().getId()){
                                    bet.toString();
                                    cout << "\n";
                                }
                            }
                        }
                        break;
                    case 4:
                        if(user.has_value()) {
                            user = userServices.findById(conn, user.value().getId());
                            switch(account()) {
                            case 1: 
                                userServices.deposit(conn, user.value());
                                break;
                            case 2: 
                                userServices.withdraw(conn, user.value());    
                                break;
                            case 3:
                                linesFormat("CONTA");
                                cout << "R$ " << fixed << setprecision(2) << user.value().getBalance() << endl;
                                break;
                            case 0: break;
                            default: 
                                altLinesFormat("Digite uma opção válida"); 
                                break;
                            }
                        }
                        break;
                    case 0:
                        altLinesFormat("Saindo...Sentiremos sua falta;-;");
                        break;
                    default:
                        altLinesFormat("Digite uma opção válida");
                        break;
                }
            } while(option != 0);
        }
    } catch (const std::exception& e) {
        std::cerr << "Erro ao conectar com o banco de dados: " << e.what() << std::endl;
    }
    return 0;
}
