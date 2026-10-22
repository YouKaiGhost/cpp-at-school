#include <iostream>
#include <limits>
#include <memory>
#include <string>

class Medicine {
protected:
    std::string name;

public:
    explicit Medicine(const std::string& name) : name(name) {}
    virtual ~Medicine() = default;
    virtual void use() const = 0;
    const std::string& getName() const { return name; }
};

class HealthPotion : public Medicine {
public:
    HealthPotion() : Medicine("恢复药水") {}
    void use() const override {
        std::cout << "服用" << name << "，恢复 50 点生命值。\n";
    }
};

class ManaPotion : public Medicine {
public:
    ManaPotion() : Medicine("魔力药水") {}
    void use() const override {
        std::cout << "服用" << name << "，恢复 30 点法力值。\n";
    }
};

class Shop {
public:
    Medicine* buyMedicine(int choice) const {
        switch (choice) {
        case 1:
            return new HealthPotion();
        case 2:
            return new ManaPotion();
        default:
            return nullptr;
        }
    }
};

class Backpack {
private:
    Medicine** medicines;
    int capacity;
    int count;

    void expandCapacity() {
        const int newCapacity = capacity * 2;
        Medicine** newMedicines = new Medicine*[newCapacity];
        for (int i = 0; i < count; ++i) {
            newMedicines[i] = medicines[i];
        }
        delete[] medicines;
        medicines = newMedicines;
        capacity = newCapacity;
    }

public:
    Backpack() : medicines(new Medicine*[4]), capacity(4), count(0) {}

    ~Backpack() {
        for (int i = 0; i < count; ++i) {
            delete medicines[i];
        }
        delete[] medicines;
    }

    // 背包拥有药物对象，禁止复制，避免同一组指针被重复释放。
    Backpack(const Backpack&) = delete;
    Backpack& operator=(const Backpack&) = delete;

    // 调用后由背包接管 medicine 的所有权；传入空指针时只提示，不保存。
    bool addMedicine(Medicine* medicine) {
        if (medicine == nullptr) {
            std::cout << "无效的药物选择。\n";
            return false;
        }

        std::unique_ptr<Medicine> guard(medicine);
        if (count == capacity) {
            expandCapacity();
        }
        medicines[count++] = guard.release();
        std::cout << "已放入背包：" << medicine->getName() << "\n";
        return true;
    }

    bool useMedicine(int index) {
        if (index < 0 || index >= count) {
            std::cout << "无效的药物编号。\n";
            return false;
        }

        medicines[index]->use();
        delete medicines[index];
        for (int i = index; i < count - 1; ++i) {
            medicines[i] = medicines[i + 1];
        }
        --count;
        medicines[count] = nullptr;
        return true;
    }

    void listMedicines() const {
        if (count == 0) {
            std::cout << "背包为空。\n";
            return;
        }
        for (int i = 0; i < count; ++i) {
            std::cout << i + 1 << ". " << medicines[i]->getName() << "\n";
        }
    }

    bool empty() const { return count == 0; }
};

bool readInt(const std::string& prompt, int& value) {
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return true;
        }
        if (std::cin.eof()) {
            return false;
        }
        std::cout << "输入无效，请输入整数。\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

int main() {
    Shop shop;
    Backpack backpack;

    while (true) {
        std::cout << "\n===== 商店与背包 =====\n"
                  << "1. 购买药物\n"
                  << "2. 查看背包\n"
                  << "3. 服用药物\n"
                  << "0. 退出\n";
        int choice = 0;
        if (!readInt("请选择：", choice)) {
            std::cout << "\n输入结束，退出。\n";
            break;
        }

        if (choice == 0) {
            std::cout << "游戏结束。\n";
            break;
        }

        switch (choice) {
        case 1: {
            std::cout << "\n1. 恢复药水（恢复 50 点生命值）\n"
                      << "2. 魔力药水（恢复 30 点法力值）\n"
                      << "0. 取消\n";
            int medicineChoice = 0;
            if (!readInt("请选择：", medicineChoice)) {
                std::cout << "\n输入结束，退出。\n";
                return 0;
            }
            if (medicineChoice == 0) {
                break;
            }
            backpack.addMedicine(shop.buyMedicine(medicineChoice));
            break;
        }
        case 2:
            backpack.listMedicines();
            break;
        case 3: {
            if (backpack.empty()) {
                std::cout << "背包为空，无法服用。\n";
                break;
            }
            backpack.listMedicines();
            int index = 0;
            if (!readInt("请输入药物编号（0 取消）：", index)) {
                std::cout << "\n输入结束，退出。\n";
                return 0;
            }
            if (index == 0) {
                break;
            }
            backpack.useMedicine(index - 1);
            break;
        }
        default:
            std::cout << "无效选择。\n";
            break;
        }
    }

    return 0;
}
