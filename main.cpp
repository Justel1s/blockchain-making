#include <bits/stdc++.h>
#include <chrono>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <unordered_map>
#include <random>
#include <algorithm>

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
    User() {}  // Default constructor   
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

        // Explicitly create a User object and add it to the vector
        users.push_back(User(name, publicKey, balance));
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

    bool tryMining(int maxAttempts, int timeLimitSeconds) {
        std::string target(difficulty, '0');
        auto startTime = std::chrono::steady_clock::now();
        int attempts = 0;

        while (attempts < maxAttempts) {
            nonce++;
            blockHash = calculateHash();
            attempts++;

            if (blockHash.substr(0, difficulty) == target) {
                std::cout << "Blokas iskastas su hash'u: " << blockHash << " per " << attempts << " bandymus." << std::endl;
                return true;
            }

            auto currentTime = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count() >= timeLimitSeconds) {
                break;
            }
        }
        return false;
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

/// BLOCKCHAIN KLASE
class Blockchain {
private:
    std::vector<Block> blocks;
    int difficulty;
    std::map<std::string, User> users;

public:
    Blockchain(const std::vector<User>& initialUsers, int difficulty = 4) : difficulty(difficulty) {
        for (const auto& user : initialUsers) {
            users[user.getPublicKey()] = user;
        }

        Block genesisBlock("0", {}, 0, difficulty);
        blocks.push_back(genesisBlock);
    }

    void addBlock(Block& newBlock, std::vector<Transaction>& transactions, std::ostream& out) {
        for (const auto& tx : newBlock.getTransactions()) {
            User& sender = users[tx.getSender()];
            User& receiver = users[tx.getReceiver()];

            if (sender.getBalance() >= tx.getAmount()) {
                sender.updateBalance(-tx.getAmount());
                receiver.updateBalance(tx.getAmount());

                transactions.erase(std::remove(transactions.begin(), transactions.end(), tx), transactions.end());
            }
        }

        blocks.push_back(newBlock);
        newBlock.printBlock(out);
    }

    const std::string& getLastBlockHash() const { return blocks.back().getBlockHash(); }
    int getDifficulty() const { return difficulty; }

    void listAllTransactions() const {
        for (const auto& block : blocks) {
            for (const auto& tx : block.getTransactions()) {
                tx.printTransaction(std::cout);
            }
        }
    }

    void listAllBlocks() const {
        for (const auto& block : blocks) {
            block.printBlock(std::cout);
        }
    }

    void listAllAccounts() const {
        for (const auto& [publicKey, user] : users) {
            user.printUser(std::cout);
        }
    }
};

/// MAIN
int main() {
    srand(static_cast<unsigned int>(time(0)));
    std::ofstream outFile("blokai.txt");

    std::vector<User> users = generateUsers(1000);
    Blockchain blockchain(users);
    int blockHeight = 1;
    int maxAttempts = 100000;
    int timeLimitSeconds = 5;  

    while (true) {
        
        std::vector<Transaction> transactions = generateTransactions(users, 100);

        while (!transactions.empty()) {
            bool blockMined = false; 

            while (!blockMined) {
                std::vector<Block> candidates;

                for (int i = 0; i < 5; ++i) {
                    std::vector<Transaction> selectedTransactions = selectRandomTransactions(transactions, 100);
                    Block candidateBlock(blockchain.getLastBlockHash(), selectedTransactions, blockHeight, blockchain.getDifficulty());
                    candidates.push_back(candidateBlock);
                }

                for (auto& candidate : candidates) {
                    if (candidate.tryMining(maxAttempts, timeLimitSeconds)) {
                        blockchain.addBlock(candidate, transactions, outFile);
                        blockHeight++;
                        blockMined = true;
                        break;  
                    }
                }

                if (!blockMined) {
                    maxAttempts *= 2;
                    timeLimitSeconds += 2;
                    std::cout << "No block mined, increasing attempt count and time limit." << std::endl;
                }
            }
        }

        // User menu for further interactions
        while (true) {
            std::cout << "\nMenu options:\n1. View all transactions\n2. View all blocks\n3. Generate 1000 new users and 10000 new transactions\n4. View all accounts\n0. Exit\nChoose: ";
            int choice;
            std::cin >> choice;

            if (choice == 1) {
                blockchain.listAllTransactions();
            } else if (choice == 2) {
                blockchain.listAllBlocks();
            } else if (choice == 3) {
                std::vector<User> newUsers = generateUsers(1000);
                users.insert(users.end(), newUsers.begin(), newUsers.end());
                break;
            } else if (choice == 4) {
                blockchain.listAllAccounts();
            } else if (choice == 0) {
                outFile.close();
                return 0;
            } else {
                std::cout << "Invalid choice. Please try again." << std::endl;
            }
        }
    }
}
