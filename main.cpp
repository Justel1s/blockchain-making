#include <bits/stdc++.h>

/// HASHAVIMO GENERATORIUS
std::string toHexString(uint64_t value, int width = 16) {
    std::stringstream stream;
    stream << std::hex << std::setw(width) << std::setfill('0') << value;
    return stream.str();
}

std::string hashavimoGeneratorius(const std::string &input) {
    uint64_t vardas = 74117108105;
    uint64_t pavarde = 10111611683;
    uint64_t hashavimas1 = 0x20043112;
    uint64_t hashavimas2 = 0x19760223;
    uint64_t hashavimas3 = 0x12345678;
    uint64_t hashavimas4 = 0x9abcdef0;

    uint64_t pirminis1 = 149837;
    uint64_t pirminis2 = 458611;
    uint64_t pirminis3 = 823541;
    uint64_t pirminis4 = 1299709;

    for (char c : input) {
        hashavimas1 ^= static_cast<uint64_t>(c);
        hashavimas1 *= pirminis1;
        hashavimas2 ^= static_cast<uint64_t>(c);
        hashavimas2 *= pirminis2;
        hashavimas3 ^= static_cast<uint64_t>(c);
        hashavimas3 *= pirminis3;
        hashavimas4 ^= static_cast<uint64_t>(c);
        hashavimas4 *= pirminis4;
    }

    hashavimas1 ^= (hashavimas2 >> 20);
    hashavimas2 ^= (hashavimas1 << 26);
    hashavimas3 ^= (hashavimas4 >> 15);
    hashavimas4 ^= (hashavimas3 << 31);

    hashavimas1 *= vardas;
    hashavimas2 *= pavarde;
    hashavimas3 *= pavarde;
    hashavimas4 *= vardas;

    return toHexString(hashavimas1) + toHexString(hashavimas2) + toHexString(hashavimas3) + toHexString(hashavimas4);
}

/// NAUDOTOJO KLASE
class User {
private:
    std::string name;
    std::string publicKey;
    int balance;

public:
    User(const std::string& name, const std::string& publicKey, int balance) 
        : name(name), publicKey(publicKey), balance(balance) {}

    const std::string& getPublicKey() const { return publicKey; }
    int getBalance() const { return balance; }
    void updateBalance(int amount) { balance += amount; }

    void printUser(std::ostream& out) const {
        out << "Vardas: " << name << ", Public Key: " << publicKey << ", Balansas: " << balance << std::endl;
    }
};

/// RANDOM VARTOTOJU KURIMAS
std::vector<User> generateUsers(int numUsers = 1000) {
    std::vector<User> users;
    for (int i = 0; i < numUsers; ++i) {
        std::string name = "Vartotojas" + std::to_string(i);
        std::string publicKey = hashavimoGeneratorius(name);
        int balance = rand() % 1000000 + 100;
        users.emplace_back(name, publicKey, balance);
    }
    return users;
}

/// TRANSAKCIJOS KLASE
class Transaction {
private:
    std::string transactionID;
    std::string sender;
    std::string receiver;
    int amount;

public:
    Transaction() : transactionID(""), sender(""), receiver(""), amount(0) {}
    Transaction(const std::string& sender, const std::string& receiver, int amount) 
        : sender(sender), receiver(receiver), amount(amount) {
        transactionID = hashavimoGeneratorius(sender + receiver + std::to_string(amount));
    }

    const std::string& getTransactionID() const { return transactionID; }
    const std::string& getSender() const { return sender; }
    const std::string& getReceiver() const { return receiver; }
    int getAmount() const { return amount; }

    void printTransaction(std::ostream& out) const {
        out << "Transakcijos ID: " << transactionID << ", Nuo: " << sender << ", Kam: " << receiver << ", Kiekis: " << amount << std::endl;
    }

    bool operator==(const Transaction& other) const {
        return transactionID == other.transactionID;
    }
};

/// RANDOM 10000 TRANSKACIJU KURIMAS
std::vector<Transaction> generateTransactions(const std::vector<User>& users, int numTransactions = 10000) {
    std::vector<Transaction> transactions;
    for (int i = 0; i < numTransactions; ++i) {
        const User& sender = users[rand() % users.size()];
        const User& receiver = users[rand() % users.size()];
        if (sender.getPublicKey() != receiver.getPublicKey()) {
            int amount = rand() % sender.getBalance() + 1;
            transactions.emplace_back(sender.getPublicKey(), receiver.getPublicKey(), amount);
        }
    }
    return transactions;
}

/// RANDOM TRANSAKCIJU PASIRINKIMAS
std::vector<Transaction> selectRandomTransactions(const std::vector<Transaction>& transactions, int count = 100) {
    std::vector<Transaction> selectedTransactions;
    std::sample(transactions.begin(), transactions.end(), std::back_inserter(selectedTransactions), count, std::mt19937{std::random_device{}()});
    return selectedTransactions;
}

/// BLOKO KLASE
class Block {
private:
    std::string prevHash;
    std::string blockHash;
    std::string merkleRoot;
    std::vector<Transaction> transactions;
    int nonce;
    int difficulty;
    time_t miningDate;
    int height;

    /// MERKLE MEDZIO SAKNIES APSKAICIAVIMO FUNKCIJA
    std::string calculateMerkleRoot() const {
        if (transactions.empty()) return "";
        std::vector<std::string> merkle;
        for (const auto& tx : transactions)
            merkle.push_back(tx.getTransactionID());

        while (merkle.size() > 1) {
            if (merkle.size() % 2 != 0) {
                merkle.push_back(merkle.back());
            }

            std::vector<std::string> newMerkle;
            for (size_t i = 0; i < merkle.size(); i += 2) {
                newMerkle.push_back(hashavimoGeneratorius(merkle[i] + merkle[i + 1]));
            }
            merkle = newMerkle;
        }
        return merkle[0];
    }

    std::string calculateHash() const {
        std::string combinedData = prevHash + std::to_string(nonce) + merkleRoot;
        return hashavimoGeneratorius(combinedData);
    }

public:
    Block(const std::string& prevHash, const std::vector<Transaction>& transactions, int height, int difficulty = 4)
        : prevHash(prevHash), transactions(transactions), nonce(0), difficulty(difficulty), height(height) {
        miningDate = std::time(nullptr);
        merkleRoot = calculateMerkleRoot();
        blockHash = calculateHash();
    }

    const std::string& getBlockHash() const { return blockHash; }
    const std::vector<Transaction>& getTransactions() const { return transactions; }
    int getHeight() const { return height; }

    void mineBlock(std::ostream& out) {
        std::string target(difficulty, '0');
        do {
            nonce++;
            blockHash = calculateHash();
        } while (blockHash.substr(0, difficulty) != target);
        out << "Blokas iskastas su hash'u: " << blockHash << std::endl;
    }

    void printBlock(std::ostream& out) const {
        out << "Block Height: " << height << std::endl;
        out << "Bloko hash'as: " << blockHash << std::endl;
        out << "Praeitas hash'as: " << prevHash << std::endl;
        out << "Merkle Saknis: " << merkleRoot << std::endl;
        out << "Transakciju kiekis: " << transactions.size() << std::endl;
        out << "Sudetingumas: " << difficulty << std::endl;
        out << "Nonce: " << nonce << std::endl;
        out << "Kasimo laikas: " << std::ctime(&miningDate) << std::endl;
        out << "Transakcijos:" << std::endl;

        for (const auto& tx : transactions) {
            tx.printTransaction(out);
        }
        out << "---------------------------------------------" << std::endl;
    }
};

/// BLOCKCHAINO KLASE
class Blockchain {
private:
    std::vector<Block> chain;
    std::unordered_map<std::string, User*> userMap;
    std::unordered_map<std::string, Transaction> allTransactions;
public:
    Blockchain(const std::vector<User>& users) {
        for (const auto& user : users) {
            userMap[user.getPublicKey()] = new User(user);
        }
    }
    std::string getLastBlockHash() const {
        return chain.empty() ? "0" : chain.back().getBlockHash();
    }
    void addBlock(Block& block, std::vector<Transaction>& pendingTransactions, std::ostream& out) {
        block.mineBlock(out);
        chain.push_back(block);
        block.printBlock(out);
        for (const auto& tx : block.getTransactions()) {
            allTransactions[tx.getTransactionID()] = tx;
            User* sender = userMap[tx.getSender()];
            User* receiver = userMap[tx.getReceiver()];
            if (sender && receiver && sender->getBalance() >= tx.getAmount()) {
                sender->updateBalance(-tx.getAmount());
                receiver->updateBalance(tx.getAmount());
            }
        }
        auto end = std::remove_if(pendingTransactions.begin(), pendingTransactions.end(),
                                  [&block](const Transaction& tx) {
                                      return std::find(block.getTransactions().begin(), block.getTransactions().end(), tx) != block.getTransactions().end();
                                  });
        pendingTransactions.erase(end, pendingTransactions.end());
    }
    void listAllTransactions() const;
    void listAllBlocks() const;
    void listAllAccounts() const;
};

// VISU TRANSKACIJU SPAUSINDIMO FUNKCIJA
void Blockchain::listAllTransactions() const {
    std::cout << "Visos transakcijos:" << std::endl;
    for (const auto& [id, transaction] : allTransactions) {
        std::cout << "Transakcijos ID: " << id << std::endl;
    }
    std::cout << "Iveskite Transakcijos ID perziureti issamia informacija arba 'exit' kad grizti i pagrindini meniu: ";
    std::string transactionID;
    std::cin >> transactionID;
    if (transactionID != "exit") {
        auto it = allTransactions.find(transactionID);
        if (it != allTransactions.end()) {
            it->second.printTransaction(std::cout);
        } else {
            std::cout << "Transakcija nerasta." << std::endl;
        }
    }
}

// VISU BLOKU SPAUSINDIMO FUNKCIJA
void Blockchain::listAllBlocks() const {
    std::cout << "Visi blokai:" << std::endl;
    for (size_t i = 0; i < chain.size(); ++i) {
        std::cout << "Blokas " << i + 1 << std::endl;
    }
    std::cout << "Iveskite bloko numeri perziureti issamia informacija arba '0' kad grizti i pagrindini meniu: ";
    int blockNumber;
    std::cin >> blockNumber;
    if (blockNumber > 0 && blockNumber <= chain.size()) {
        chain[blockNumber - 1].printBlock(std::cout);
    } else if (blockNumber != 0) {
        std::cout << "Neteisingas bloko numeris." << std::endl;
    }

}
// VISU PASKYRU SPAUSINDIMO FUNKCIJA
void Blockchain::listAllAccounts() const {
    std::cout << "Visos paskyros:" << std::endl;
    for (const auto& [publicKey, user] : userMap) {
        user->printUser(std::cout);
    }
    std::cout << "---------------------------------------------" << std::endl;
}

/// PAGRINDINE FUNKCIJA
int main() {
    srand(static_cast<unsigned int>(time(0)));
    /// BLOKU SPAUSDINIMO FAILAS
    std::ofstream outFile("blokai.txt");

    std::vector<User> users = generateUsers(1000);
    Blockchain blockchain(users);
    int blockHeight = 1;

    while (true) {
        std::vector<Transaction> transactions = generateTransactions(users, 10000);

        while (!transactions.empty()) {
            int transactionCount = std::min(100, static_cast<int>(transactions.size()));
            std::vector<Transaction> selectedTransactions = selectRandomTransactions(transactions, transactionCount);
            Block newBlock(blockchain.getLastBlockHash(), selectedTransactions, blockHeight++);
            blockchain.addBlock(newBlock, transactions, outFile);
        }

        /// MENIU
        while (true) {
            std::cout << std::endl;
            std::cout << "Meniu pasirinkimai:" << std::endl;
            std::cout << "1. Perziureti visas transakcijas" << std::endl;
            std::cout << "2. Perziureti visus blokus" << std::endl;
            std::cout << "3. Generuoti 1000 nauju naudotoju ir 10000 nauju transkaciju" << std::endl;
            std::cout << "4. Perziureti visas paskyras" << std::endl;
            std::cout << "0. Iseiti" << std::endl;
            std::cout << "Pasirinkite: ";

            int choice;
            std::cin >> choice;

            if (choice == 1) {
                blockchain.listAllTransactions();
            } 
            else if (choice == 2) {
                blockchain.listAllBlocks();
            } 
            else if (choice == 3) {
                std::vector<User> newUsers = generateUsers(1000);
                users.insert(users.end(), newUsers.begin(), newUsers.end());
                break;
            } 
            else if (choice == 4) {
                blockchain.listAllAccounts();
            } 
            else if (choice == 0) {
                outFile.close();
                return 0;
            } 
            else {
                std::cout << "Blogas pasirinkimas. Bandykite is naujo." << std::endl;
            }
        }
    }
}