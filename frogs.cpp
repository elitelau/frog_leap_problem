/*
 *  frog game introduction:
 *
 *  there are 6 frogs aligned like following:
 *  OOOXooo
 *  ... ...
 *  123 321  (number on frogs)
 *
 *  0123456  (number overall position)
 *
 *  O: represents frogs on the left, while
 *  o: represents frogs on the right.
 *  both O and o jump towards and cannot get back by auxiliary a gapped position X with values 4
 *  for example: 
 *  O3 jumps to G4(gapped with position number 4), after that frogs aligned like following:
 *  OOXOooo
 *  o3 jumps to G3, after that frogs aligned like:
 *  OOoOXoo
 * 
 */

#include <algorithm>
#include <vector>
#include <list>
#include <queue>
#include <iostream>
#include <cassert>

typedef std::pair<int, int> EntityId;

enum {
    LEFT_FROG = 0,
    RIGHT_FROG = 1,
    GAP
};

const char* g_array_entity_denote[] = {
    "L",
    "R",
    "G"
};

static std::vector<EntityId> g_array_entity_id = 
    {
        {LEFT_FROG, 1},
        {LEFT_FROG, 2},
        {LEFT_FROG, 3},
        {GAP, 0},
        {RIGHT_FROG, 3},
        {RIGHT_FROG, 2},
        {RIGHT_FROG, 1}
    };

struct ssgVertex;
typedef std::vector<EntityId*> EntityArray;             // position array
typedef std::pair<EntityArray, struct ssgVertex*> State;    // state enumerated in whole state space 

typedef struct ssgVertex {
    ssgVertex(State* s, ssgVertex* p, unsigned int num)
        : state(s), parent(p), child_ref_number(num)
    {}
    State* state;
    struct ssgVertex* parent;
    // number of child in breadth traversal, and it will
    // decreases 1 when a child is popped from queue
    unsigned int    child_ref_number;  
} ssgVertex;


void copy_value(const State* from, State* to) {
   to->second = from->second;
   for (auto x : from->first) {
      to->first.push_back(x);
   }
}

inline void print_state(const State* s, const std::string& additional = std::string()) {
    for (auto elem: s->first) {
        std::cout << g_array_entity_denote[elem->first] << elem->second << " ";
    }
    std::cout << additional << std::endl;
    fflush(stdout);
}

void get_step_desc(const State* prev_state, const State* cur_state, std::string& out);

void print_solution(const State* s) {
    std::list<const State*>   solution;
    std::string         step_desc;
    for(ssgVertex* v = s->second; v  !=  NULL;   v   = v->parent) {
        solution.push_back(v->state);
    }
    solution.reverse();

    std::cout << "solution path is:" << std::endl;
    std::list<const State*>::iterator it = solution.begin();
    print_state(*it);
    const State* prev_state = *it;
    for (++it; it != solution.end(); prev_state = *it, ++it) {
        get_step_desc(prev_state, *it, step_desc);
        print_state(*it, step_desc);
    }
}

inline void get_step_desc(const State* prev_state, const State* cur_state, std::string& out) {
    int from_index = -1, to_index = -1, diff_count = 0;
    for (int i = 0; i < prev_state->first.size(); ++i) {
        if (prev_state->first[i]->first != cur_state->first[i]->first &&
            prev_state->first[i]->second != cur_state->first[i]->second)
        {
            if (prev_state->first[i]->first == GAP) {
                to_index = i;
            }
            else {
                from_index = i; 
            }
            ++diff_count;
        }
    }
    assert(diff_count == 2 && from_index * to_index >= 0);

    char desc[100] = {'\0'};
    sprintf(desc, 
            "(frog %s%d[%d] leaps to gap[%d])", 
            g_array_entity_denote[prev_state->first[from_index]->first],
            prev_state->first[from_index]->second,
            from_index,
            to_index);
    out = desc;
}

// delete a subtree backtraced from current node which should be popped off
inline void delete_bfs_node(State* cur_stat) {
    ssgVertex* v = cur_stat->second;
    ssgVertex* parent = NULL;

    do {
        parent = v->parent;
        delete v->state;
        delete v;
        v = parent;
    }
    while (v && --v->child_ref_number == 0);
}

inline bool determine_destination(const State* s) {
    std::string form = "";
    for (auto x: s->first) {
        form += g_array_entity_denote[x->first];
    }

    if (form == "RRRGLLL") {
        return true;
    }
    return false;
}

void jump(size_t frog_index, size_t gap_index, EntityArray& array) {
    EntityId* temp = array[frog_index];

    array[frog_index] = array[gap_index];
    array[gap_index] = temp;
}

void attach_vertex(ssgVertex* parent, State* transit_stat) {
    ssgVertex* v = new ssgVertex(transit_stat, parent, 0);
    transit_stat->second = v;
    ++parent->child_ref_number;
}

// list possible states that current state goes to
void list_next_states(State* cur_stat, std::list<State*>& next_stat_lst) {
   int       gap_index       = -1;
   int       i               = 0;
   int       frog_index      = -1;
   unsigned int child_ref_number    = 0;

   for (i = 0; i < cur_stat->first.size(); ++i) {
      if (cur_stat->first[i]->first == GAP) {
         gap_index = i;
         break;
      }
   }
  
   // check a frog that can jump to the gap from left side
   if (gap_index > 0 &&
       cur_stat->first[gap_index-1]->first == LEFT_FROG) 
   {
      frog_index = gap_index - 1;
      State* transit_stat = new State();
      copy_value(cur_stat, transit_stat);
      jump(frog_index, gap_index, transit_stat->first);
      next_stat_lst.push_back(transit_stat);

      attach_vertex(cur_stat->second, transit_stat); 
   }

   if (gap_index - 2 >= 0 && 
       cur_stat->first[gap_index-2]->first == LEFT_FROG) 
   {
      frog_index = gap_index -2;
      State* transit_stat = new State();
      copy_value(cur_stat, transit_stat);
      jump(frog_index, gap_index, transit_stat->first); 
      next_stat_lst.push_back(transit_stat);
   
      attach_vertex(cur_stat->second, transit_stat); 
   }
   
   // check a frog that can jump to the gap from right side
   if (gap_index < g_array_entity_id.size() - 1 &&
       cur_stat->first[gap_index+1]->first == RIGHT_FROG) 
   {
      frog_index = gap_index + 1;
      State* transit_stat = new State();
      copy_value(cur_stat, transit_stat);
      jump(frog_index, gap_index, transit_stat->first);
      next_stat_lst.push_back(transit_stat);
   
      attach_vertex(cur_stat->second, transit_stat); 
   }

   if (gap_index + 2 <= g_array_entity_id.size() - 1 &&
       cur_stat->first[gap_index+2]->first == RIGHT_FROG) 
   {
      frog_index = gap_index + 2;
      State* transit_stat = new State();
      copy_value(cur_stat, transit_stat);
      jump(frog_index, gap_index, transit_stat->first);
      next_stat_lst.push_back(transit_stat);
   
      attach_vertex(cur_stat->second, transit_stat); 
   }

#if 0  
   std::cout << "from state:" << std::endl;
   print_state(cur_stat);
   std::cout << "to states:" << std::endl;
   
   for (auto x: next_stat_lst) {
      print_state(x);
   }

   std::cout << "-------------------" << std::endl;
#endif
}

void frog_problem() {
    State first_stat = {
                           {&g_array_entity_id[0],
                            &g_array_entity_id[1],
                            &g_array_entity_id[2],
                            &g_array_entity_id[3],
                            &g_array_entity_id[4],
                            &g_array_entity_id[5],
                            &g_array_entity_id[6]
                           },
                           NULL
                       };

    State* init_stat = new State();
    copy_value(&first_stat, init_stat);

    ssgVertex* root = new ssgVertex(init_stat, NULL, 0);
    init_stat->second = root;

    std::queue<State*> bfs_queue;
    bfs_queue.push(init_stat);
        
    std::list<State*> next_stat_lst;

    while (!bfs_queue.empty()) {
        State* cur_stat = bfs_queue.front();
        bfs_queue.pop();

        list_next_states(cur_stat, next_stat_lst);

        if (next_stat_lst.size() > 0) {
            for (auto x: next_stat_lst) {
                bfs_queue.push(x);
                if (determine_destination(x)) {
                    print_solution(x);
                } 
            }
        }
        else {
            delete_bfs_node(cur_stat);
        }

        next_stat_lst.clear();
    }
}

int main(int argc, const char** argv) {
    frog_problem();
    return 0;
}

