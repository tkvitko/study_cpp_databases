//
//  main.cpp
//  homework_07
//
//  Created by tkvitko on 16.04.2024.
//

#include <iostream>
#include <string>
#include <vector>
#include <pqxx/pqxx>


struct Client{
    std::string firstname;
    std::string lastname;
    std::string email;
    std::vector<std::string> phones;
};


class ClientManager {
private:
    pqxx::connection * conn = nullptr;
    
public:
    ClientManager() {
        try {
            conn = new pqxx::connection("host=localhost "
                                      "port=5432 "
                                      "dbname=clients "
                                      "user=postgres ");
        } catch (pqxx::sql_error e) {
            std::cout << e.what() << std::endl;
        }
    };
    
    ~ClientManager() {
        conn = nullptr;
        delete conn;
    }
    
    void create_db() {
        pqxx::transaction<> tx{ *conn };
        
        // таблица клиентов
        tx.exec("CREATE TABLE IF NOT EXISTS clients ( "
                "id SERIAL primary key, "
                "firstname varchar(32), "
                "lastname varchar(32), "
                "email varchar(64) "
                ");");
        
        // таблица телефонов
        tx.exec("CREATE TABLE IF NOT exists client_phones ( "
                "id SERIAL primary key, "
                "client_id INTEGER not null references clients(id), "
                "phone varchar(10) not null unique "
                ");");
        
        tx.commit();
    }

    void add_client(const std::string firstname, const std::string lastname, const std::string email) {
        // добавить нового клиента
        
        pqxx::transaction<> tx{ *conn };
        tx.exec("insert into clients (firstname, lastname, email)"
                "values ('" + tx.esc(firstname) + "', '" + tx.esc(lastname) + "', '" + tx.esc(email) + "');");
        tx.commit();
    }

    void add_phone(const std::string firstname, const std::string lastname, const std::string phone) {
        // добавить телефон клиенту
        
        pqxx::transaction<> tx{ *conn };
        tx.exec("insert into client_phones (client_id, phone)"
                "values ((select id from clients where lastname = '" + tx.esc(lastname) + "' and firstname = '" +  tx.esc(firstname) + "'), " + tx.esc(phone) + ");");
        tx.commit();
    }

    void change_client_lastname(const std::string firstname, const std::string lastname, const std::string new_lastname) {
        // изменить фамилию клиента
        
        pqxx::transaction<> tx{ *conn };
        tx.exec("update clients "
                "set lastname = '" + tx.esc(new_lastname) + "' where lastname = '" + tx.esc(lastname) + "' and firstname = '" + tx.esc(firstname) + "';");
        tx.commit();
    }

    void delete_clients_phones(const std::string firstname, const std::string lastname) {
        // удалить телефоны клиента
        
        pqxx::transaction<> tx{ *conn };
        tx.exec("delete from client_phones "
                "where client_id = (select id from clients where lastname = '" + tx.esc(lastname) + "' and firstname = '" + tx.esc(firstname) + "');");
        tx.commit();
    }

    void delete_client(const std::string firstname, const std::string lastname) {
        // удалить клиента
        
        pqxx::transaction<> tx{ *conn };
        tx.exec("delete from clients "
                "where lastname = '" + tx.esc(lastname) + "' and firstname = '" + tx.esc(firstname) + "';");
        tx.commit();
    }

    int search_client(const std::string field, const std::string value) {
        // найти клиента
        
        pqxx::transaction<> tx{ *conn };
        int client_id = tx.query_value<int>("select id from clients "
                            "where " + tx.esc(field) + " = '" + tx.esc(value) + "';");
        return client_id;
    }
};

int main(int argc, const char * argv[]) {
    
    ClientManager manager = ClientManager();
        
    manager.create_db();
    
    Client demo;
    demo.firstname = "Taras";
    demo.lastname = "Kvitko";
    demo.email = "tkvitko@gmail.com";
    demo.phones.push_back("9110231144");
    
    manager.add_client(demo.firstname, demo.lastname, demo.email);
    manager.add_phone(demo.firstname, demo.lastname, demo.phones[0]);
    
    std::string old_lastname = demo.lastname;
    demo.lastname = "Ivanov";
    manager.change_client_lastname(demo.firstname, old_lastname, demo.lastname);
    
    manager.delete_clients_phones(demo.firstname, demo.lastname);
    int client_id = manager.search_client("firstname", demo.firstname);
    std::cout << client_id << std::endl;
    manager.delete_client(demo.firstname, demo.lastname);
        
    return 0;
}
