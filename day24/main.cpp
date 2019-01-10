/* 
 * File:   main.cpp
 * Author: dseres
 *
 * Created on 2019. január 4., 23:47
 */

#include <bits/stdc++.h>

using namespace std;

enum AttackType : int {
    Cold = 0, Fire, Radiation, Slashing, Bludgeoning, MaxAttackType, UndefAttackType = -1
};

enum GroupType : int {
    ImmuneSystem = 0, Infection, MaxGroupType, UndefGroupType = -1
};

ostream &operator<<(ostream &strm, GroupType t) {
    if (t == GroupType::ImmuneSystem)
        return strm << "Immune system";
    if (t == GroupType::Infection)
        return strm << "Infection";
    return strm << "Undefined";
}

struct TargetInfo {
    int possible_damage = 0;
    int effective_power = 0;
    int initiative = 0;

    friend bool operator<(const TargetInfo &t, const TargetInfo &u) {
        return make_tuple(t.possible_damage, t.effective_power, t.initiative) < make_tuple(u.possible_damage, u.effective_power, u.initiative);
    }

    friend bool operator>(const TargetInfo &t, const TargetInfo &u) {
        return u<t;
    }

    friend bool operator==(const TargetInfo &t, const TargetInfo &u) {
        return make_tuple(t.possible_damage, t.effective_power, t.initiative) == make_tuple(u.possible_damage, u.effective_power, u.initiative);
    }
};

struct Group {
    GroupType type;
    int units;
    int hp;
    int damage;
    AttackType attack;
    int initiative;
    array<bool, AttackType::MaxAttackType> weaknesses;
    array<bool, AttackType::MaxAttackType> immunes;
    int targetIndex;
    bool isTarget;

    Group() : type(GroupType::UndefGroupType), units(0), hp(0), damage(0), attack(AttackType::UndefAttackType)
    , initiative(0), targetIndex(0), isTarget(false) {
        weaknesses.fill(false);
        immunes.fill(false);
    }
    Group(const Group &) = default;
    Group(Group &&) = default;
    Group &operator=(const Group &) = default;
    Group &operator=(Group &&) = default;
    ~Group() = default;

    int getIndex() const {
        return initiative - 1;
    }

    int getEffectivePower() const {
        return units*damage;
    }

    TargetInfo testOpponent(Group op) const {
        TargetInfo t;
        t.possible_damage = 0;
        t.effective_power = op.getEffectivePower();
        t.initiative = op.initiative;
        if (op.isImmune(attack))
            return t;
        t.possible_damage = this->getEffectivePower();
        if (op.isWeak(attack))
            t.possible_damage *= 2;
        return t;
    }

    bool isOpponent(const Group &g) const {
        return ( type == GroupType::ImmuneSystem && g.type == GroupType::Infection)
                || (g.type == GroupType::ImmuneSystem && type == GroupType::Infection);
    }

    bool isDied() const {
        return units == 0;
    }

    bool isImmune(AttackType a) const {
        return immunes[a];
    }

    bool isWeak(AttackType a) const {
        return weaknesses[a];
    }

    bool hasTarget() const {
        return 0 <= targetIndex;
    }

    void applyDamage(const TargetInfo &own) {
        int killed = own.possible_damage / hp;
        units = max(units - killed, 0);
    }

    friend bool operator<(const Group &g, const Group &h) {
        return g.initiative < h.initiative;
    };

    friend bool operator>(const Group &g, const Group &h) {
        return g.initiative > h.initiative;
    };

    friend bool operator==(const Group &g, const Group &h) {
        return g.initiative == h.initiative;
    };
};

struct BattleSimulator {
    vector<Group> initial_groups;
    vector<Group> groups;
    vector<Group*> groups_for_target_selection;

    BattleSimulator(vector<Group> initial_groups) : initial_groups(initial_groups) {
    }

    int computeMinBoost() {
        int boost = 0;
        GroupType winner=GroupType::UndefGroupType;
        cout << "In computeMinBoost." << endl;
        while (winner != GroupType::ImmuneSystem) {
            battle(++boost);
            int units;
            tie(winner, units) = getBattleResult();
            std::cout << "The winner is: " << /*Mc Hammer*/winner << ", remaining units: " << units << endl;
        }
        return boost;
    }

    void battle(int boost = 0) {
        groups = initial_groups;
        groups_for_target_selection.reserve(groups.size());
        applyBoost(boost);
        //std::cout << "Boost: " <<boost<< endl;
        while (countAliveGroups() > 0) {
            if (!selectTargetForAll()) {
                break; //Not found target -> Stalemate
            }
            attack();
        }
    }

    void applyBoost(int boost) {
        for (Group &g : groups)
            if (g.type == GroupType::ImmuneSystem)
                g.damage += boost;
    }

    bool selectTargetForAll() {
        bool has_target = false;
        for (Group &g : groups) {
            g.targetIndex = -1;
            g.isTarget = false;
        }
        sortGroupsForTargetSelecting();
        for (auto &g : groups_for_target_selection) {
            if (selectTarget(*g)) {
                has_target = true;
            }
        }
        return has_target;
    }

    void sortGroupsForTargetSelecting() {
        groups_for_target_selection.clear();
        for(Group &g: groups)
            if ( !g.isDied())
                groups_for_target_selection.push_back(&g);
            
        //copy_if(groups.begin(),groups.end(), back_inserter(groups_for_target_selection), [](Group g){return !g.isDied();});
        auto cmp = [](const Group *g, const Group *h) {
            return make_tuple(g->getEffectivePower(), g->initiative) > make_tuple(h->getEffectivePower(), h->initiative);
        };
        sort(groups_for_target_selection.begin(), groups_for_target_selection.end(), cmp);
    }

    bool selectTarget(const Group &attacker) {
        bool found = false;
        TargetInfo target;
        for (const Group *o : groups_for_target_selection) {
            const Group &opponent=*o;
            if (attacker.isOpponent(opponent) && !opponent.isTarget) {
                TargetInfo t = attacker.testOpponent(opponent);
                if (t.possible_damage > 0) {
                    if (!found || target < t) {
                        groups[attacker.getIndex()].targetIndex = opponent.getIndex();
                        found = true;
                        target = t;
                    }
                }
            }
        }
        if (found){
            int target_index = groups[attacker.getIndex()].targetIndex;
            groups[target_index].isTarget = true;
        }
        return found;
    }

    void attack() {
        for (auto it = groups.rbegin(); it != groups.rend(); ++it) {
            Group &attacker = *it;
            if (!attacker.isDied() && attacker.hasTarget()) {
                Group &target = groups[attacker.targetIndex];
                auto target_info = attacker.testOpponent(target);
                target.applyDamage(target_info);
            }
        }
    }

    pair<GroupType, int> getBattleResult() {
        int count_immune = countUnits(GroupType::ImmuneSystem);
        int count_infection = countUnits(GroupType::Infection);
        if (count_immune == 0 && count_infection > 0)
            return make_pair(GroupType::Infection, count_infection);
        else if (count_infection == 0 && count_immune > 0)
            return make_pair(GroupType::ImmuneSystem, count_immune);
        else
            return make_pair(GroupType::UndefGroupType, -1);
    }

    int countUnits(GroupType type) {
        int units = 0;
        for (Group g : groups)
            if (g.type == type && !g.isDied())
                units += g.units;
        return units;
    }

    int countAliveGroups() {
        return count_if(groups.begin(), groups.end(), [](Group g) {
            return !g.isDied();
        });
    }
};

class InputParser {
public:

    static BattleSimulator readFrom(const string & input) {
        istringstream strm(input);
        return readFrom(strm);
    }

    static BattleSimulator readFrom(istream & input) {
        vector<Group> groups;
        GroupType type = GroupType::UndefGroupType;
        for (string line; getline(input, line);) {
            if (!line.empty()) {
                if (line == "Immune System:") {
                    type = GroupType::ImmuneSystem;
                } else if (line == "Infection:") {
                    type = GroupType::Infection;
                } else {
                    Group g = parseGroup(line);
                    g.type = type;
                    if (groups.size() <= static_cast<unsigned> (g.getIndex())) {
                        groups.resize(g.getIndex() + 1);
                    }
                    groups[g.getIndex()] = g;
                }
            }
        }
        groups.shrink_to_fit();
        return BattleSimulator(groups);
    }

private:

    static Group parseGroup(const string &line) {
        Group g;
        string str = "^(\\d+) units each with (\\d+) hit points( \\([,;\\w ]*\\))? with an attack that does (\\d+)"
                " (cold|fire|radiation|slashing|bludgeoning) damage at initiative (\\d+)$";
        static regex r(str);
        smatch m;
        if (regex_match(line, m, r)) {
            g.units = stoi(m[1]);
            g.hp = stoi(m[2]);
            g.weaknesses.fill(false);
            g.immunes.fill(false);
            parseImpacts(&g, m[3]);
            g.damage = stoi(m[4]);
            g.attack = toAttack(m[5]);
            g.initiative = stoi(m[6]);
        } else {
            cerr << "Bad input: " << line << endl;
            exit(-1);
        }
        return g;
    }

    static void parseImpacts(Group *g, string str) {
        if (str.length() > 0) {
            static regex delimiters(R"([\(\)\s,;]+|\s*to\s*)");
            str = regex_replace(str, delimiters, " ");
            istringstream strm(str);
            auto *impacts = &(g->weaknesses);
            for (string s; strm >> s;) {
                if (s == "weak") {
                    impacts = &(g->weaknesses);
                } else if (s == "immune") {
                    impacts = &(g->immunes);
                } else {
                    impacts->at(toAttack(s)) = true;
                    ;
                }
            }
        }
    }

    static AttackType toAttack(string str) {
        AttackType type = AttackType::UndefAttackType;
        if (str == "cold") type = Cold;
        else if (str == "fire") type = Fire;
        else if (str == "radiation") type = Radiation;
        else if (str == "slashing") type = Slashing;
        else if (str == "bludgeoning") type = Bludgeoning;
        return type;
    }

};

void unitTest() {

    string input = R"(Immune System:
17 units each with 5390 hit points (weak to radiation, bludgeoning) with an attack that does 4507 fire damage at initiative 2
989 units each with 1274 hit points (immune to fire; weak to bludgeoning, slashing) with an attack that does 25 slashing damage at initiative 3

Infection:
801 units each with 4706 hit points (weak to radiation) with an attack that does 116 bludgeoning damage at initiative 1
4485 units each with 2961 hit points (immune to radiation; weak to fire, cold) with an attack that does 12 slashing damage at initiative 4)";
    BattleSimulator smltr = InputParser::readFrom(input);
    smltr.battle();
    GroupType winner;
    int units;
    tie(winner, units) = smltr.getBattleResult();
    std::cout << "The winner is: " << /*Mc Hammer*/winner << ", remaining units: " << units << endl;
    assert(units == 5216 && winner == GroupType::Infection);

    smltr.battle(1570);
    tie(winner, units) = smltr.getBattleResult();
    std::cout << "The winner is: " << /*Mc Hammer*/winner << ", remaining units: " << units << endl;
    assert(units == 51 && winner == GroupType::ImmuneSystem);

    int min_boost = smltr.computeMinBoost();
    cout << "Min boost: " << min_boost << endl;
    assert(min_boost == 1570);
}

void computeSolution1() {
    ifstream input("input.txt");
    BattleSimulator smltr = InputParser::readFrom(input);
    smltr.battle();
    GroupType winner;
    int units;
    tie(winner, units) = smltr.getBattleResult();
    std::cout << "The winner is: " << /*Mc Hammer*/winner << ", remaining units: " << units << endl;
    int min_boost = smltr.computeMinBoost();
    cout << "Min boost: " << min_boost << endl;
}

/*
 * 
 */
int main() {
    unitTest();
    computeSolution1();
    return 0;
}

