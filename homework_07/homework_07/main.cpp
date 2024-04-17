//
//  main.cpp
//  homework_07
//
//  Created by tkvitko on 16.04.2024.
//

#include <iostream>
#include <string>
#include <pqxx/pqxx>


void create_db(pqxx::connection& conn) {
    pqxx::transaction<> tx{ conn };
    
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

void add_client(pqxx::connection& conn, std::string firstname, std::string lastname, std::string email) {
    // добавить нового клиента
    
    pqxx::transaction<> tx{ conn };
    tx.exec("insert into clients (firstname, lastname, email)"
            "values ('" + tx.esc(firstname) + "', '" + tx.esc(lastname) + "', '" + tx.esc(email) + "');");
    tx.commit();
}

void add_phone(pqxx::connection& conn, std::string firstname, std::string lastname, std::string phone) {
    // добавить телефон клиенту
    
    pqxx::transaction<> tx{ conn };
    tx.exec("insert into client_phones (client_id, phone)"
            "values ((select id from clients where lastname = '" + tx.esc(lastname) + "' and firstname = '" +  tx.esc(firstname) + "'), " + tx.esc(phone) + ");");
    tx.commit();
}

void change_client_lastname(pqxx::connection& conn, std::string firstname, std::string lastname, std::string new_lastname) {
    // изменить фамилию клиента
    
    pqxx::transaction<> tx{ conn };
    tx.exec("update clients "
            "set lastname = '" + tx.esc(new_lastname) + "' where lastname = '" + tx.esc(lastname) + "' and firstname = '" + tx.esc(firstname) + "';");
    tx.commit();
}

void delete_clients_phones(pqxx::connection& conn, std::string firstname, std::string lastname) {
    // удалить телефоны клиента
    
    pqxx::transaction<> tx{ conn };
    tx.exec("delete from client_phones "
            "where client_id = (select id from clients where lastname = '" + tx.esc(lastname) + "' and firstname = '" + tx.esc(firstname) + "');");
    tx.commit();
}

void delete_client(pqxx::connection& conn, std::string firstname, std::string lastname) {
    // удалить клиента
    
    pqxx::transaction<> tx{ conn };
    tx.exec("delete from clients "
            "where lastname = '" + tx.esc(lastname) + "' and firstname = '" + tx.esc(firstname) + "';");
    tx.commit();
}

int search_client(pqxx::connection& conn, std::string field, std::string value) {
    // найти клиента
    
    pqxx::transaction<> tx{ conn };
    int client_id = tx.query_value<int>("select id from clients "
                        "where " + tx.esc(field) + " = '" + tx.esc(value) + "';");
    return client_id;
}


int main(int argc, const char * argv[]) {
    
    try
    {
        pqxx::connection conn(
                              "host=localhost "
                              "port=5432 "
                              "dbname=clients "
                              "user=postgres ");
        
        create_db(conn);
        add_client(conn, "Taras", "Kvitko", "tkvitko@gmail.com");
        add_phone(conn, "Taras", "Kvitko", "9110231144");
        change_client_lastname(conn, "Taras", "Kvitko", "Ivanov");
        delete_clients_phones(conn, "Taras", "Ivanov");
        int client_id = search_client(conn, "firstname", "Taras");
        std::cout << client_id << std::endl;
        delete_client(conn, "Taras", "Ivanov");
        
        
    } catch (pqxx::sql_error e) {
        std::cout << e.what() << std::endl;
    }
    
    return 0;
}
