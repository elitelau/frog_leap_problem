/*
 *  frog game introduction:
 *
 *  there are 6 frogs aligned like following:
 *  OOOXooo
 *  ... ...
 *  123 321  (number on frogs)
 *
 *  1234567  (number overall position)
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
    
// ? how to define global enumeration type values
// use closure? define State() within frog_problem() internal?
var LEFT_FROG = 0, RIGHT_FROG = 1, GAP = 2; 
var g_entity_denote = ['L', 'R', 'x'];

function State(d, v) {
    this.data = d;
    this.vertex = v;

    this.get_next_states = function(next_stat_list) {
        var i = 0; 
        var gap_index = -1;
        for (; i < this.data.length; ++i) {
            if (this.data[i].type === GAP) {
                gap_index = i;
                break;
            }
        }

        if (gap_index > 0 &&
            this.data[gap_index-1].type === LEFT_FROG) 
        {
            var frog_index = gap_index -1;
            add_new_state.call(this, frog_index, gap_index, next_stat_list);
        }

        if (gap_index -2 >= 0 &&
            this.data[gap_index-2].type === LEFT_FROG)
        {
            var frog_index = gap_index -2;
            add_new_state.call(this, frog_index, gap_index, next_stat_list);
        }

        if (gap_index < this.data.length -1 &&
            this.data[gap_index+1].type === RIGHT_FROG)
        {
            var frog_index = gap_index + 1;
            add_new_state.call(this, frog_index, gap_index, next_stat_list);
        }

        if (gap_index +2 <= this.data.length -1 &&
            this.data[gap_index+2].type === RIGHT_FROG)
        {
            var frog_index = gap_index + 2;
            add_new_state.call(this, frog_index, gap_index, next_stat_list);
        }
    }

    this.reach_destination = function() {
        var i = 0;
        var form = '';
        for (; i < this.data.length; i = i + 1) {
            form += g_entity_denote[this.data[i].type];
        }

        return form === 'RRRxLLL';
    }

    this.get_howto_step = function(prev_stat) {
        var from_index = -1, to_index = -1, diff_count = 0;
        var i = 0;
        for (; i < prev_stat.data.length; ++i) {
            if (prev_stat.data[i].type !== this.data[i].type &&
                prev_stat.data[i].num !== this.data[i].num)
            {
                if (prev_stat.data[i].type === GAP) {
                    to_index = i;
                }
                else {
                    from_index = i;
                }
                diff_count++;
            }
        }
        console.assert(diff_count === 2 && to_index * from_index >= 0);

        var step_desc_str = "frog '" + g_entity_denote[prev_stat.data[from_index].type]  
                            + prev_stat.data[from_index].num + "' leaps to gap at position "
                            + (to_index +1);
        return step_desc_str;
    }

    this.print = function(additional) {
        var i = 0;
        var str = '';
        for (; i < this.data.length; i = i + 1) {
            str += g_entity_denote[this.data[i].type] + this.data[i].num + ' ';
        }
        if (additional) {
            str += '(' + additional + ')';
        }
        console.log(str);
    }

    var add_new_state = function (frog_index, gap_index, next_stat_list) {
        var d = this.data.slice();   // clone array with value copy
        var transit_stat = new State(d, null);
        jump.call(transit_stat, frog_index, gap_index);
        attach_vertex.call(transit_stat, this.vertex);

        next_stat_list.push(transit_stat);
    }
    
    var jump = function (frog_index, gap_index) {
        var temp = this.data[frog_index];
        this.data[frog_index] = this.data[gap_index];
        this.data[gap_index] = temp;
    }

    var attach_vertex = function (parent_vertex) {
        var v = new Vertex(this, parent_vertex);
        this.vertex = v;
        parent_vertex.child_ref_number++;
    }
}

function Vertex(s, p) {
    this.state = s;
    this.parent = p;
    this.child_ref_number = 0;
}

function print_solution(dest_stat) {
    var state_list = [];
    var v = dest_stat.vertex;
    for (; v; v = v.parent) {
        state_list.push(v.state);
    }
    state_list.reverse();

    console.log('solution path is:');

    var i = 0;
    state_list[0].print();
    var prev_stat = state_list[0];
    for (++i; i < state_list.length; prev_stat = state_list[i], ++i) {
       var step_desc_str = state_list[i].get_howto_step(prev_stat);
       state_list[i].print(step_desc_str)
    }
}

function delete_bfs_node(popoff_stat) {
    var v = popoff_stat.vertex;
    var parent = null;

    do {
        parent = v.parent;
        v.state = null; // free memory for reuse
        v = null; // free memory for reuse
        v = parent;
    } while (v && --v.child_ref_number == 0);
}

var frog_problem = function() {
    var entity_id_array = [ {type: LEFT_FROG, num:1}, {type: LEFT_FROG, num: 2}, 
                            {type: LEFT_FROG, num: 3}, {type: GAP, num: 0},
                            {type: RIGHT_FROG, num: 3}, {type: RIGHT_FROG, num: 2},
                            {type: RIGHT_FROG, num: 1}
                          ]; 
   
    // bfs on possible states, and construct a state space tree
    // a state has a bidirectional association with a tree node
    var init_stat =  new State([entity_id_array[0], entity_id_array[1], entity_id_array[2],
                                entity_id_array[3], entity_id_array[4], entity_id_array[5], 
                                entity_id_array[6]
                               ], null);
    var root = new Vertex(init_stat, null);
    init_stat.vertex = root;

    var bfs_queue = [];
    bfs_queue.push(init_stat);

    while(bfs_queue.length) {
        var top_stat = bfs_queue.shift();

        var next_stat_list = [];
        top_stat.get_next_states(next_stat_list);

        if (next_stat_list.length) {
            var i = 0; 
            for (; i < next_stat_list.length; ++i) {
                bfs_queue.push(next_stat_list[i]);

                if (next_stat_list[i].reach_destination()) {
                    print_solution(next_stat_list[i]);
                }
            }
        }
        else {
            delete_bfs_node(top_stat);
        }
    }
};

frog_problem();


