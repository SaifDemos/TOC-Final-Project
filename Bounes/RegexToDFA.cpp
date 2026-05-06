#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <stack>
#include <string>
#include <algorithm>
#include <limits>
#include <cctype>
#include <fstream>

using namespace std;

// ============================================================================
// DATA STRUCTURES
// ============================================================================
struct NFA {
    int numStates = 0;
    set<int> startStates;
    set<int> finalStates;
    set<char> alphabet;
    map<int, map<char, set<int>>> transitions;
    map<int, set<int>> epsilonTransitions;
};

struct DFA {
    int numStates = 0;
    int startState = -1;
    set<int> finalStates;
    set<char> alphabet;
    map<int, map<char, int>> transitions;
};

// ============================================================================
// UTILITIES
// ============================================================================
void clearBuffer() { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); }
void pressEnter() { cout << "\n[Press Enter to continue...]"; clearBuffer(); cin.get(); }

// Compute ε-closure using BFS (manual loops only)
set<int> epsilonClosure(const NFA& nfa, const set<int>& states) {
    set<int> closure;
    // Manual copy of states to closure
    for (int s : states) closure.insert(s);
    
    queue<int> q;
    for (int s : states) q.push(s);
    
    while (!q.empty()) {
        int curr = q.front(); q.pop();
        auto it = nfa.epsilonTransitions.find(curr);
        if (it != nfa.epsilonTransitions.end()) {
            // Manual loop through epsilon targets
            for (int nxt : it->second) {
                if (closure.find(nxt) == closure.end()) {
                    closure.insert(nxt);
                    q.push(nxt);
                }
            }
        }
    }
    return closure;
}

bool isValidRegex(const string& re) {
    if (re.empty()) return false;
    int bal = 0;
    for (char c : re) {
        if (c == '(') bal++;
        else if (c == ')') bal--;
        if (bal < 0) return false;
        if (!isalnum(c) && c != '_' && c != '-' && c != '|' && c != '*' && 
            c != '+' && c != '?' && c != '(' && c != ')' && c != '.') return false;
    }
    return bal == 0;
}

// ============================================================================
// REGEX PARSER
// ============================================================================
string preprocessRegex(const string& re) {
    string res;
    for (size_t i = 0; i < re.size(); ++i) {
        res += re[i];
        if (i + 1 < re.size()) {
            char c = re[i], n = re[i+1];
            bool cEnd = (isalnum(c) || c == '_' || c == '-' || c == ')' || c == '*' || c == '+' || c == '?');
            bool nStart = (isalnum(n) || n == '_' || n == '-' || n == '(');
            if (cEnd && nStart) res += '.';
        }
    }
    return res;
}

string toPostfix(const string& re) {
    string p = preprocessRegex(re);
    string out; stack<char> st;
    auto prec = [](char op){ return op=='|' ? 1 : op=='.' ? 2 : 0; };
    for (char c : p) {
        if (c == '(') st.push(c);
        else if (c == ')') { while(!st.empty() && st.top()!='('){ out+=st.top(); st.pop(); } if(!st.empty()) st.pop(); }
        else if (c == '|' || c == '.') { while(!st.empty() && st.top()!='(' && prec(st.top())>=prec(c)){ out+=st.top(); st.pop(); } st.push(c); }
        else if (c=='*' || c=='+' || c=='?') out += c;
        else out += c;
    }
    while(!st.empty()){ out+=st.top(); st.pop(); }
    return out;
}

// ============================================================================
// THOMPSON'S CONSTRUCTION (Regex → NFA)
// ============================================================================
class Thompson {
private:
    static NFA literal(char c) {
        NFA n; n.numStates=2; n.startStates={0}; n.finalStates={1}; 
        n.alphabet={c}; n.transitions[0][c].insert(1); return n;
    }
    static NFA empty() {
        NFA n; n.numStates=2; n.startStates={0}; n.finalStates={1}; 
        n.epsilonTransitions[0].insert(1); return n;
    }
    
    // Manual copy of NFA with offset
    static void copyNFA(const NFA& src, int off, NFA& dst) {
        dst.numStates += src.numStates;
        // Manual copy of transitions
        for (auto& e : src.transitions) {
            int state = e.first;
            for (auto& kv : e.second) {
                char sym = kv.first;
                for (int t : kv.second) {
                    dst.transitions[state + off][sym].insert(t + off);
                }
            }
        }
        // Manual copy of epsilon transitions
        for (auto& e : src.epsilonTransitions) {
            int state = e.first;
            for (int t : e.second) {
                dst.epsilonTransitions[state + off].insert(t + off);
            }
        }
        // Manual copy of alphabet
        for (char c : src.alphabet) dst.alphabet.insert(c);
    }

    static NFA concat(const NFA& a, const NFA& b) {
        NFA r; r.numStates = 0;
        copyNFA(a, 0, r);
        int off = a.numStates;
        copyNFA(b, off, r);
        r.startStates = a.startStates;
        // Connect a finals to b starts via ε
        for (int f : a.finalStates) {
            for (int s : b.startStates) {
                r.epsilonTransitions[f].insert(s + off);
            }
        }
        r.finalStates.clear();
        for (int f : b.finalStates) r.finalStates.insert(f + off);
        return r;
    }

    static NFA unionNFA(const NFA& a, const NFA& b) {
        int o1 = 1, o2 = 1 + a.numStates;
        int finalId = 1 + a.numStates + b.numStates;  // ← Fixed: store final state ID
        
        NFA r; 
        r.startStates = {0};
        r.finalStates = {finalId};  // ← Fixed: use stored ID
        r.numStates = finalId + 1;  // ← Fixed: use stored ID
        
        for (int s : a.startStates) r.epsilonTransitions[0].insert(s + o1);
        for (int s : b.startStates) r.epsilonTransitions[0].insert(s + o2);
        
        copyNFA(a, o1, r);
        copyNFA(b, o2, r);
        
        // ← Fixed: use stored finalId
        for (int f : a.finalStates) r.epsilonTransitions[f + o1].insert(finalId);
        for (int f : b.finalStates) r.epsilonTransitions[f + o2].insert(finalId);
        
        return r;
    }

    static NFA kleene(const NFA& a) {
        NFA r; 
        int newStart = 0;
        int newFinal = 1 + a.numStates;
        r.startStates = {newStart};
        r.finalStates = {newFinal};
        r.numStates = newFinal + 1;
        int off = 1;

        for (int s : a.startStates) r.epsilonTransitions[newStart].insert(s + off);
        r.epsilonTransitions[newStart].insert(newFinal);

        copyNFA(a, off, r);

        for (int f : a.finalStates) {
            r.epsilonTransitions[f + off].insert(newFinal);
            for (int s : a.startStates) {
                r.epsilonTransitions[f + off].insert(s + off);
            }
        }
        return r;
    }

public:
    static NFA build(string regex) {
        string pf = toPostfix(regex);
        stack<NFA> stk; set<char> alpha;
        for(char c:pf){
            if(isalnum(c)||c=='_'||c=='-'){ alpha.insert(c); stk.push(literal(c)); }
            else if(c=='.'){ NFA b=stk.top();stk.pop(); NFA a=stk.top();stk.pop(); stk.push(concat(a,b)); }
            else if(c=='|'){ NFA b=stk.top();stk.pop(); NFA a=stk.top();stk.pop(); stk.push(unionNFA(a,b)); }
            else if(c=='*'){ NFA a=stk.top();stk.pop(); stk.push(kleene(a)); }
            else if(c=='+'){ NFA a=stk.top();stk.pop(); stk.push(concat(a,kleene(a))); }
            else if(c=='?'){ NFA a=stk.top();stk.pop(); stk.push(unionNFA(a,empty())); }
        }
        NFA res=stk.top(); res.alphabet=alpha; return res;
    }
};

// ============================================================================
// SUBSET CONSTRUCTION (NFA → DFA)
// ============================================================================
DFA subsetConstruct(const NFA& nfa) {
    DFA dfa; dfa.alphabet=nfa.alphabet;
    map<set<int>,int> subMap; queue<set<int>> q; int nid=0;
    set<int> start=epsilonClosure(nfa,nfa.startStates);
    subMap[start]=nid++; dfa.startState=0; q.push(start);
    
    cout << "[INFO] Building DFA states with ε-closure...\n";
    bool f=true; cout << "  Start: {"; for(int s:start){cout<<(f?"":", ")<<s;f=false;} cout<<"} → DFA q0\n";
    
    while(!q.empty()){
        set<int> cur=q.front();q.pop(); int cid=subMap[cur];
        for(int s:cur) if(nfa.finalStates.find(s)!=nfa.finalStates.end()){dfa.finalStates.insert(cid);break;}
        
        for(char sym:dfa.alphabet){
            set<int> moveSet;
            for(int st:cur){
                auto it=nfa.transitions.find(st);
                if(it!=nfa.transitions.end()){
                    auto sit=it->second.find(sym);
                    if(sit!=it->second.end()){
                        // Manual loop to merge targets
                        for(int t : sit->second) moveSet.insert(t);
                    }
                }
            }
            set<int> nxt=epsilonClosure(nfa,moveSet);
            if(!nxt.empty()){
                if(subMap.find(nxt)==subMap.end()){ subMap[nxt]=nid++; q.push(nxt);
                    cout << "  [NEW] DFA q" << subMap[nxt] << " = ε-closure(move({"; f=true; for(int s:cur){cout<<(f?"":", ")<<s;f=false;}
                    cout << "}, '" << sym << "')) = {"; f=true; for(int s:nxt){cout<<(f?"":", ")<<s;f=false;} cout<<"}\n";
                }
                dfa.transitions[cid][sym]=subMap[nxt];
            }
        }
    }
    dfa.numStates=nid;
    cout << "[SUCCESS] DFA has " << dfa.numStates << " states.\n";
    return dfa;
}

// ============================================================================
// DFA MINIMIZATION
// ============================================================================
DFA minimizeDFA(const DFA& dfa) {
    if(dfa.numStates<=2) return dfa;
    vector<set<int>> part;
    set<int> F=dfa.finalStates, NF;
    for(int i=0;i<dfa.numStates;++i) if(F.find(i)==F.end()) NF.insert(i);
    if(!F.empty()) part.push_back(F); if(!NF.empty()) part.push_back(NF);
    
    bool changed=true;
    while(changed){
        changed=false; vector<set<int>> newP;
        for(const auto& g:part){
            if(g.size()<=1){newP.push_back(g);continue;}
            map<vector<int>,set<int>> sig;
            for(int s:g){
                vector<int> v;
                for(char c:dfa.alphabet){
                    auto it=dfa.transitions.find(s);
                    int target = -1;
                    if(it!=dfa.transitions.end()){
                        auto sit=it->second.find(c);
                        if(sit!=it->second.end()) target=sit->second;
                    }
                    int gid=-1;
                    for(size_t i=0;i<part.size();++i) if(part[i].find(target)!=part[i].end()){gid=(int)i;break;}
                    v.push_back(gid);
                }
                sig[v].insert(s);
            }
            if(sig.size()>1){ changed=true; for(auto&kv:sig) newP.push_back(kv.second); } else newP.push_back(g);
        }
        if(changed) part=move(newP);
    }
    
    DFA m; m.alphabet=dfa.alphabet; map<int,int> toG;
    for(size_t i=0;i<part.size();++i){
        for(int s:part[i]) toG[s]=(int)i;
        if(part[i].find(dfa.startState)!=part[i].end()) m.startState=(int)i;
        for(int s:part[i]) if(dfa.finalStates.find(s)!=dfa.finalStates.end()){m.finalStates.insert((int)i);break;}
    }
    for(size_t i=0;i<part.size();++i){
        if(!part[i].empty()){
            int r=*part[i].begin();
            for(char c:dfa.alphabet){
                auto it=dfa.transitions.find(r);
                if(it!=dfa.transitions.end()){
                    auto sit=it->second.find(c);
                    if(sit!=it->second.end()) m.transitions[(int)i][c]=toG[sit->second];
                }
            }
        }
    }
    m.numStates=(int)part.size(); return m;
}

// ============================================================================
// SIMULATION & VISUALIZATION
// ============================================================================
bool simulate(const DFA& dfa, const string& inp) {
    int cur=dfa.startState;
    cout << "\n[TRACE] Input: \"" << inp << "\"\n  Start: q" << cur << "\n";
    for(char c:inp){
        auto it=dfa.transitions.find(cur);
        if(it!=dfa.transitions.end()){
            auto sit=it->second.find(c);
            if(sit!=it->second.end()){
                cur=sit->second;
                cout << "  --'" << c << "'--> q" << cur << "\n";
                continue;
            }
        }
        cout << "  --'" << c << "'--> ✗ REJECTED\n"; return false;
    }
    bool acc=dfa.finalStates.find(cur)!=dfa.finalStates.end();
    cout << "  Final: q" << cur << " | " << (acc ? "✓ ACCEPTED" : "✗ REJECTED") << "\n"; return acc;
}

void drawNFA(const NFA& nfa, const string& regex) {
    cout << "\n[STEP 1/4] NFA: \"" << regex << "\"\n" << string(60,'=') << "\n";
    cout << "States: "; for(int i=0;i<nfa.numStates;++i) cout << (nfa.startStates.find(i)!=nfa.startStates.end()?"->":"  ") << (nfa.finalStates.find(i)!=nfa.finalStates.end()?"[*]":"[ ]") << "q" << i << "  ";
    cout << "\nAlphabet: {"; bool f=true; for(char c:nfa.alphabet){cout<<(f?"":", ")<<c;f=false;} cout<<"}\n\nTransitions:\n";
    for(int i=0;i<nfa.numStates;++i){
        for(char sym:nfa.alphabet){ 
            auto it=nfa.transitions.find(i); 
            if(it!=nfa.transitions.end()){ 
                auto sit=it->second.find(sym); 
                if(sit!=it->second.end()) for(int t:sit->second) cout<<"  q"<<i<<" --"<<sym<<"--> q"<<t<<"\n"; 
            } 
        }
        auto eit=nfa.epsilonTransitions.find(i); 
        if(eit!=nfa.epsilonTransitions.end()) for(int t:eit->second) cout<<"  q"<<i<<" --ε--> q"<<t<<"\n";
    }
}

void drawDFA(const DFA& dfa, bool mini=false) {
    cout << "\n[STEP " << (mini?"3":"2") << "/4] DFA" << (mini?" (Minimized)":"") << "\n" << string(60,'=') << "\n";
    cout << "States: "; for(int i=0;i<dfa.numStates;++i) cout << (i==dfa.startState?"->":"  ") << (dfa.finalStates.find(i)!=dfa.finalStates.end()?"[*]":"[ ]") << "q" << i << "  ";
    cout << "\nAlphabet: {"; bool f=true; for(char c:dfa.alphabet){cout<<(f?"":", ")<<c;f=false;} cout<<"}\n\nTransition Table:\n";
    cout << "+-------"; for(size_t i=0;i<dfa.alphabet.size();++i)cout<<"+-------"; cout<<"+\n| State ";
    for(char c:dfa.alphabet)cout<<"|  "<<c<<"  "; cout<<"|\n+-------";
    for(size_t i=0;i<dfa.alphabet.size();++i)cout<<"+-------"; cout<<"+\n";
    for(int i=0;i<dfa.numStates;++i){ cout<<"|  q"<<i<<" "; for(char c:dfa.alphabet){ auto it=dfa.transitions.find(i); if(it!=dfa.transitions.end()){auto sit=it->second.find(c);if(sit!=it->second.end())cout<<"|  q"<<sit->second<<" ";else cout<<"|  -  ";}else cout<<"|  -  ";} cout<<"|\n"; }
    cout<<"+-------"; for(size_t i=0;i<dfa.alphabet.size();++i)cout<<"+-------"; cout<<"+\n";
}

void exportDOT(const DFA& dfa, const string& fname) {
    ofstream out(fname+".dot"); out<<"digraph DFA {\n  rankdir=LR;\n  node [shape=circle];\n  start [shape=point];\n";
    for(int s:dfa.finalStates) out<<"  "<<s<<" [shape=doublecircle];\n";
    out<<"  start -> "<<dfa.startState<<" [style=invis];\n";
    for(auto&[st,mp]:dfa.transitions) for(auto&[sym,t]:mp) out<<"  "<<st<<" -> "<<t<<" [label=\""<<sym<<"\"];\n";
    out<<"}\n"; out.close(); cout << "[INFO] DOT saved: " << fname << ".dot\n";
}

// ============================================================================
// MAIN
// ============================================================================
int main() {
    cout << "\n" << string(60,'=') << "\n  TOC Final Project 2026 - Bonus\n  Regex → NFA → DFA → Simulator\n" << string(60,'=') << "\n";
    cout << "Try: (a|b)*abb, a*b*, (a|b)+ab, 0|1*0, a?a*b\n" << string(60,'=') << "\n";
    pressEnter();

    string regex; while(true){
        cout << "\nEnter Regex: "; cin >> regex;
        if(regex.empty()){ cout << "   [ERROR] Empty."; continue; }
        if(!isValidRegex(regex)){ cout << "   [ERROR] Invalid syntax."; continue; } break;
    }

    cout << "\n[STEP 1/4] Building NFA...\n"; NFA nfa = Thompson::build(regex); drawNFA(nfa, regex); pressEnter();
    cout << "\n[STEP 2/4] Converting NFA → DFA...\n"; DFA dfa = subsetConstruct(nfa); drawDFA(dfa); pressEnter();
    cout << "\n[STEP 3/4] Minimizing DFA...\n"; DFA minDfa = minimizeDFA(dfa);
    if(minDfa.numStates < dfa.numStates){ cout << "[INFO] Reduced: " << dfa.numStates << " → " << minDfa.numStates << "\n"; drawDFA(minDfa, true); }
    else { cout << "[INFO] Already minimal.\n"; } pressEnter();

    cout << "\n[STEP 4/4] Test Strings (enter '0' to finish)\n" << string(60,'=') << "\n";
    while(true){
        cout << "> "; string inp; cin >> inp; if(inp=="0") break; if(inp.empty()) continue;
        bool valid=true; for(char c:inp) if(minDfa.alphabet.find(c)==minDfa.alphabet.end()){valid=false;break;}
        if(!valid){ cout << "   [WARN] Invalid symbols.\n"; continue; } simulate(minDfa, inp);
    }
    exportDOT(minDfa, "final_dfa"); cout << "\n✅ Done! Good luck! 🎓\n"; return 0;
}