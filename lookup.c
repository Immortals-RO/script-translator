#include "lookup.h"

int long_compare(void *, void *);

int string_store(struct string *,struct store *,  struct string **);
int string_strtod(struct string *, double *);
int string_strtol(struct string *, int, long *);
int string_strtoul(struct string *, int, unsigned long *);
int string_strtol_split(struct string *, int, char, struct store *, struct long_array **);
int string_strtol_splitv(struct string *, int, int, ...);

struct schema_markup pet_db_markup[] = {
    {1, map, 0, NULL},
    {2, map, 1, "Header"},
    {3, string, 2, "Type"},
    {3, string, 3, "Version"},
    {2, list, 4, "Body"},
    {3, map, 5, NULL},
    {4, string, 6, "Mob"},
    {4, string, 7, "TameItem"},
    {4, string, 8, "EggItem"},
    {4, string, 9, "EquipItem"},
    {4, string, 10, "FoodItem"},
    {4, string, 11, "Fullness"},
    {4, string, 12, "HungryDelay"},
    {4, string, 13, "HungerIncrease"},
    {4, string, 14, "IntimacyStart"},
    {4, string, 15, "IntimacyFed"},
    {4, string, 16, "IntimacyOverfed"},
    {4, string, 17, "IntimacyHungry"},
    {4, string, 18, "IntimacyOwnerDie"},
    {4, string, 19, "CaptureRate"},
    {4, string, 20, "SpecialPerformance"},
    {4, string, 21, "AttackRate"},
    {4, string, 22, "RetaliateRate"},
    {4, string, 23, "ChangeTargetRate"},
    {4, string, 24, "AllowAutoFeed"},
    {4, string, 25, "Script"},
    {4, string, 26, "SupportScript"},
    {4, list, 27, "Evolution"},
    {5, map, 28, NULL},
    {6, string, 29, "Target"},
    {6, list, 30, "ItemRequirements"},
    {7, map, 31, NULL},
    {8, string, 32, "Item"},
    {8, string, 33, "Amount"},
    {0, 0, 0},
};

struct schema_markup csv_markup[] = {
    {1, list, 0, NULL},
    {2, list, 1, NULL},
    {3, string, 2, NULL},
    {0, 0, 0},
};

struct schema_markup constant_db_markup[] = {
    {1, map, 0, NULL},
    {2, list, 1, "const"},
    {3, map, 2, NULL},
    {4, string, 3, "macro"},
    {4, string, 4, "value"},
    {4, string, 5, "label"},
    {4, list, 6, "range"},
    {5, string, 7, NULL},
    {0, 0, 0},
};

struct schema_markup constant_group_markup[] = {
    {1, map, 0, NULL},
    {2, list, 1, "vip_status"},
    {3, string, 2, NULL},
    {2, list, 3, "elements"},
    {3, string, 4, NULL},
    {2, list, 5, "jobs"},
    {3, string, 6, NULL},
    {2, list, 7, "sc_end"},
    {3, string, 8, NULL},
    {2, list, 9, "sizes"},
    {3, string, 10, NULL},
    {2, list, 11, "announces"},
    {3, string, 12, NULL},
    {2, list, 13, "sc_start"},
    {3, string, 14, NULL},
    {2, list, 15, "locations"},
    {3, string, 16, NULL},
    {2, list, 17, "readparam"},
    {3, string, 18, NULL},
    {2, list, 19, "itemgroups"},
    {3, string, 20, NULL},
    {2, list, 21, "races"},
    {3, string, 22, NULL},
    {2, list, 23, "effects"},
    {3, string, 24, NULL},
    {2, list, 25, "gettimes"},
    {3, string, 26, NULL},
    {2, list, 27, "classes"},
    {3, string, 28, NULL},
    {2, list, 29, "options"},
    {3, string, 30, NULL},
    {2, list, 31, "mapflags"},
    {3, string, 32, NULL},
    {0, 0, 0},
};

int long_compare(void * x, void * y) {
    long l = *((long *) x);
    long r = *((long *) y);
    return l < r ? -1 : l > r ? 1 : 0;
}

int string_store(struct string * string, struct store * store, struct string ** result) {
    int status = 0;
    struct string * object;

    object = store_object(store, sizeof(*string) + string->length + 1);
    if(!object) {
        status = panic("failed to object store object");
    } else {
        object->string = (char *) object + sizeof(*string);
        object->length = string->length;

        memcpy(object->string, string->string, string->length);
        object->string[object->length] = 0;

        *result = object;
    }

    return status;
}

int string_strtod(struct string * string, double * result) {
    int status = 0;

    long number;
    char * last;

    if(!string->length) {
        *result = 0;
    } else {
        number = strtod(string->string, &last);
        if(string->string + string->length != last) {
            status = panic("invalid '%s' in '%s'", last, string->string);
        } else {
            *result = number;
        }
    }

    return status;
}

int string_strtol(struct string * string, int base, long * result) {
    int status = 0;

    long number;
    char * last;

    if(!string->length) {
        *result = 0;
    } else {
        number = strtol(string->string, &last, base);
        if(string->string + string->length != last) {
            status = panic("invalid '%s' in '%s'", last, string->string);
        } else {
            *result = number;
        }
    }

    return status;
}

int string_strtoul(struct string * string, int base, unsigned long * result) {
    int status = 0;

    unsigned long number;
    char * last;

    if(!string->length) {
        *result = 0;
    } else {
        number = strtoul(string->string, &last, base);
        if(string->string + string->length != last) {
            status = panic("invalid '%s' in '%s'", last, string->string);
        } else {
            *result = number;
        }
    }

    return status;
}

int string_strtol_split(struct string * string, int base, char split, struct store * store, struct long_array ** result) {
    int status = 0;
    struct long_array * array;

    char * ptr;
    size_t count;
    char * end;

    ptr = string->string;
    count = 0;
    while(ptr) {
        count++;
        ptr = strchr(ptr, split);
        if(ptr)
            ptr++;
    }

    array = store_object(store, sizeof(*array));
    if(!array) {
        status = panic("failed to object store object");
    } else {
        array->count = count;
        array->array = store_object(store, sizeof(*array->array) * array->count);
        if(!array->array) {
            status = panic("failed to object store object");
        } else {
            ptr = string->string;
            count = 0;
            while(ptr && count < array->count) {
                array->array[count++] = strtol(ptr, &end, base);
                ptr = *end == split ? end + 1 : NULL;
                if(!ptr && *end)
                    status = panic("invalid string '%s' in '%s'", end, string->string);
            }

            if(status) {

            } else {
                *result = array;
            }
        }
    }

    return status;
}

int string_strtol_splitv(struct string * string, int base, int split, ...) {
    int status = 0;
    va_list args;
    long * value;
    char * ptr;
    char * end;

    va_start(args, split);
    value = va_arg(args, long *);
    ptr = string->string;
    while(value && ptr) {
        *value = strtol(ptr, &end, base);
        ptr = *end == split ? end + 1 : NULL;
        if(!ptr && *end) {
            status = panic("invalid string '%s' in '%s'", end, string->string);
        } else {
            value = va_arg(args, long *);
        }
    }
    va_end(args);

    return status;
}

int pet_db_create(struct pet_db * pet_db, size_t size, struct heap * heap) {
    int status = 0;

    if(map_create(&pet_db->map, (map_compare_cb) strcmp, heap->map_pool)) {
        status = panic("failed to create map object");
    } else {
        if(store_create(&pet_db->store, size)) {
            status = panic("failed to create store object");
        } else {
            pet_db->pet = NULL;
            pet_db->evolve = NULL;
            pet_db->require = NULL;
        }
        if(status)
            map_destroy(&pet_db->map);
    }

    return status;
}

void pet_db_destroy(struct pet_db * pet_db) {
    store_destroy(&pet_db->store);
    map_destroy(&pet_db->map);
}

void pet_db_clear(struct pet_db * pet_db) {
    pet_db->require = NULL;
    pet_db->evolve = NULL;
    pet_db->pet = NULL;
    store_clear(&pet_db->store);
    map_clear(&pet_db->map);
}

int pet_db_parse(enum parser_event event, int mark, struct string * string, void * context) {
    int status = 0;
    struct pet_db * pet_db = context;

    switch(mark) {
        case 5:
            if(event == start) {
                pet_db->pet = store_object(&pet_db->store, sizeof(*pet_db->pet));
                if(!pet_db->pet)
                    status = panic("failed to object store object");
            } else if(event == end) {
                if(!pet_db->pet->mob) {
                    status = panic("invalid string object");
                } else if(map_insert(&pet_db->map, pet_db->pet->mob->string, pet_db->pet)) {
                    status = panic("failed to insert map object");
                }
            }
            break;
        case 6: status = string_store(string, &pet_db->store, &pet_db->pet->mob); break;
        case 7: status = string_store(string, &pet_db->store, &pet_db->pet->tame_item); break;
        case 8: status = string_store(string, &pet_db->store, &pet_db->pet->egg_item); break;
        case 9: status = string_store(string, &pet_db->store, &pet_db->pet->equip_item); break;
        case 10: status = string_store(string, &pet_db->store, &pet_db->pet->food_item); break;
        case 11: status = string_strtol(string, 10, &pet_db->pet->fullness); break;
        case 12: status = string_strtol(string, 10, &pet_db->pet->hungry_delay); break;
        case 13: status = string_strtol(string, 10, &pet_db->pet->hunger_increase); break;
        case 14: status = string_strtol(string, 10, &pet_db->pet->intimacy_start); break;
        case 15: status = string_strtol(string, 10, &pet_db->pet->intimacy_fed); break;
        case 16: status = string_strtol(string, 10, &pet_db->pet->intimacy_overfed); break;
        case 17: status = string_strtol(string, 10, &pet_db->pet->intimacy_hungry); break;
        case 18: status = string_strtol(string, 10, &pet_db->pet->intimacy_owner_die); break;
        case 19: status = string_strtol(string, 10, &pet_db->pet->capture_rate); break;
        case 20: status = string_store(string, &pet_db->store, &pet_db->pet->special_performance); break;
        case 21: status = string_strtol(string, 10, &pet_db->pet->attack_rate); break;
        case 22: status = string_strtol(string, 10, &pet_db->pet->retaliate_rate); break;
        case 23: status = string_strtol(string, 10, &pet_db->pet->change_target_rate); break;
        case 24: status = string_store(string, &pet_db->store, &pet_db->pet->allow_auto_feed); break;
        case 25: status = string_store(string, &pet_db->store, &pet_db->pet->script); break;
        case 26: status = string_store(string, &pet_db->store, &pet_db->pet->support_script); break;
        case 28:
            if(event == start) {
                pet_db->evolve = store_object(&pet_db->store, sizeof(*pet_db->evolve));
                if(!pet_db->evolve)
                    status = panic("failed to object store object");
            } else if(event == end) {
                pet_db->evolve->next = pet_db->pet->evolve;
                pet_db->pet->evolve = pet_db->evolve;
            }
            break;
        case 29: status = string_store(string, &pet_db->store, &pet_db->evolve->target); break;
        case 31:
            if(event == start) {
                pet_db->require = store_object(&pet_db->store, sizeof(*pet_db->require));
                if(!pet_db->require)
                    status = panic("failed to object store object");
            } else if(event == end) {
                pet_db->require->next = pet_db->evolve->require;
                pet_db->evolve->require = pet_db->require;
            }
            break;
        case 32: status = string_store(string, &pet_db->store, &pet_db->require->item); break;
        case 33: status = string_strtol(string, 10, &pet_db->require->amount); break;
    }

    return status;
}

int item_db_create(struct item_db * item_db, size_t size, struct heap * heap) {
    int status = 0;

    if(map_create(&item_db->map_id, long_compare, heap->map_pool)) {
        status = panic("failed to create map object");
    } else {
        if(map_create(&item_db->map_aegis, (map_compare_cb) strcmp, heap->map_pool)) {
            status = panic("failed to create map object");
        } else {
            if(store_create(&item_db->store, size)) {
                status = panic("failed to create store object");
            } else {
                item_db->item = NULL;
                item_db->id = NULL;
                item_db->script = NULL;
                item_db->index = 0;
            }
            if(status)
                map_destroy(&item_db->map_aegis);
        }
        if(status)
            map_destroy(&item_db->map_id);
    }

    return status;
}

void item_db_destroy(struct item_db * item_db) {
    store_destroy(&item_db->store);
    map_destroy(&item_db->map_aegis);
    map_destroy(&item_db->map_id);
}

void item_db_clear(struct item_db * item_db) {
    item_db->index = 0;
    item_db->script = NULL;
    item_db->id = NULL;
    item_db->item = NULL;
    store_clear(&item_db->store);
    map_clear(&item_db->map_aegis);
    map_clear(&item_db->map_id);
}

int item_db_parse(enum parser_event event, int mark, struct string * string, void * context) {
    int status = 0;
    struct item_db * item_db = context;

    switch(mark) {
        case 1:
            if(event == start) {
                item_db->item = store_object(&item_db->store, sizeof(*item_db->item));
                if(!item_db->item) {
                    status = panic("failed to object store object");
                } else {
                    item_db->index = 0;
                }
            } else if(event == end) {
                if(item_db->index != 20) {
                    status = panic("invalid column");
                } else if(!item_db->item->aegis) {
                    status = panic("invalid string object");
                } else if(map_insert(&item_db->map_id, &item_db->item->id, item_db->item)) {
                    status = panic("failed to insert map object");
                } else if(map_insert(&item_db->map_aegis, item_db->item->aegis->string, item_db->item)) {
                    status = panic("failed to insert map object");
                } else {
                    item_db->item->combo = NULL;
                }
            }
            break;
        case 2:
            switch(item_db->index) {
                case 0: status = string_strtol(string, 10, &item_db->item->id); break;
                case 1: status = string_store(string, &item_db->store, &item_db->item->aegis); break;
                case 2: status = string_store(string, &item_db->store, &item_db->item->name); break;
                case 3: status = string_strtol(string, 10, &item_db->item->type); break;
                case 4: status = string_strtol(string, 10, &item_db->item->buy); break;
                case 5: status = string_strtol(string, 10, &item_db->item->sell); break;
                case 6: status = string_strtol(string, 10, &item_db->item->weight); break;
                case 7: status = string_strtol_splitv(string, 10, ':', &item_db->item->atk, &item_db->item->matk, NULL); break;
                case 8: status = string_strtol(string, 10, &item_db->item->def); break;
                case 9: status = string_strtol(string, 10, &item_db->item->range); break;
                case 10: status = string_strtol(string, 10, &item_db->item->slots); break;
                case 11: status = string_strtoul(string, 16, &item_db->item->job); break;
                case 12: status = string_strtoul(string, 10, &item_db->item->upper); break;
                case 13: status = string_strtol(string, 10, &item_db->item->gender); break;
                case 14: status = string_strtoul(string, 10, &item_db->item->location); break;
                case 15: status = string_strtol(string, 10, &item_db->item->weapon_level); break;
                case 16: status = string_strtol_splitv(string, 10, ':', &item_db->item->base_level, &item_db->item->max_level, NULL); break;
                case 17: status = string_strtol(string, 10, &item_db->item->refineable); break;
                case 18: status = string_strtol(string, 10, &item_db->item->view); break;
                case 19: status = string_store(string, &item_db->store, &item_db->item->script); break;
                default: status = panic("invalid column"); break;
            }
            item_db->index++;
            break;
    }

    return status;
}

int item_combo_db_parse(enum parser_event event, int mark, struct string * string, void * context) {
    int status = 0;
    struct item_db * item_db = context;

    size_t i;
    struct item * item;
    struct item_combo * combo;

    switch(mark) {
        case 1:
            if(event == start) {
                item_db->index = 0;
            } else {
                if(item_db->index != 2) {
                    status = panic("invalid column");
                } else {
                    for(i = 0; i < item_db->id->count && !status; i++) {
                        item = map_search(&item_db->map_id, &item_db->id->array[i]);
                        if(item) {
                            combo = store_object(&item_db->store, sizeof(*combo));
                            if(!combo) {
                                status = panic("failed to object store object");
                            } else {
                                combo->id = item_db->id;
                                combo->script = item_db->script;
                                combo->next = item->combo;
                                item->combo = combo;
                            }
                        }
                    }
                }
            }
            break;
        case 2:
            switch(item_db->index) {
                case 0: status = string_strtol_split(string, 10, ':', &item_db->store, &item_db->id); break;
                case 1: status = string_store(string, &item_db->store, &item_db->script); break;
                default: status = panic("invalid column"); break;
            }
            item_db->index++;
            break;
    }

    return status;
}

int skill_db_create(struct skill_db * skill_db, size_t size, struct heap * heap) {
    int status = 0;

    if(map_create(&skill_db->map_id, long_compare, heap->map_pool)) {
        status = panic("failed to create map object");
    } else {
        if(map_create(&skill_db->map_macro, (map_compare_cb) strcmp, heap->map_pool)) {
            status = panic("failed to create map object");
        } else {
            if(store_create(&skill_db->store, size)) {
                status = panic("failed to create store object");
            } else {
                skill_db->skill = NULL;
                skill_db->index = 0;
            }
            if(status)
                map_destroy(&skill_db->map_macro);
        }
        if(status)
            map_destroy(&skill_db->map_id);
    }

    return status;
}

void skill_db_destroy(struct skill_db * skill_db) {
    store_destroy(&skill_db->store);
    map_destroy(&skill_db->map_macro);
    map_destroy(&skill_db->map_id);
}

void skill_db_clear(struct skill_db * skill_db) {
    skill_db->index = 0;
    skill_db->skill = NULL;
    store_clear(&skill_db->store);
    map_clear(&skill_db->map_macro);
    map_clear(&skill_db->map_id);
}

int skill_db_parse(enum parser_event event, int mark, struct string * string, void * context) {
    int status = 0;
    struct skill_db * skill_db = context;

    switch(mark) {
        case 1:
            if(event == start) {
                skill_db->skill = store_object(&skill_db->store, sizeof(*skill_db->skill));
                if(!skill_db->skill) {
                    status = panic("failed to object store object");
                } else {
                    skill_db->index = 0;
                }
            } else if(event == end) {
                if(skill_db->index != 18) {
                    status = panic("invalid column");
                } else if(!skill_db->skill->macro) {
                    status = panic("invalid string object");
                } else if(map_insert(&skill_db->map_id, &skill_db->skill->id, skill_db->skill)) {
                    status = panic("failed to insert map object");
                } else if(map_insert(&skill_db->map_macro, skill_db->skill->macro->string, skill_db->skill)) {
                    status = panic("failed to insert map object");
                }
            }
            break;
        case 2:
            switch(skill_db->index) {
                case 0: status = string_strtol(string, 10, &skill_db->skill->id); break;
                case 1: status = string_strtol_split(string, 10, ':', &skill_db->store, &skill_db->skill->range); break;
                case 2: status = string_strtol(string, 10, &skill_db->skill->hit); break;
                case 3: status = string_strtol(string, 10, &skill_db->skill->inf); break;
                case 4: status = string_strtol_split(string, 10, ':', &skill_db->store, &skill_db->skill->element); break;
                case 5: status = string_strtol(string, 16, &skill_db->skill->nk); break;
                case 6: status = string_strtol_split(string, 10, ':', &skill_db->store, &skill_db->skill->splash); break;
                case 7: status = string_strtol(string, 10, &skill_db->skill->maxlv); break;
                case 8: status = string_strtol_split(string, 10, ':', &skill_db->store, &skill_db->skill->hit_amount); break;
                case 9: status = string_store(string, &skill_db->store, &skill_db->skill->cast_cancel); break;
                case 10: status = string_strtol(string, 10, &skill_db->skill->cast_def_reduce_rate); break;
                case 11: status = string_strtol(string, 16, &skill_db->skill->inf2); break;
                case 12: status = string_strtol_split(string, 10, ':', &skill_db->store, &skill_db->skill->max_count); break;
                case 13: status = string_store(string, &skill_db->store, &skill_db->skill->type); break;
                case 14: status = string_strtol_split(string, 10, ':', &skill_db->store, &skill_db->skill->blow_count); break;
                case 15: status = string_strtol(string, 16, &skill_db->skill->inf3); break;
                case 16: status = string_store(string, &skill_db->store, &skill_db->skill->macro); break;
                case 17: status = string_store(string, &skill_db->store, &skill_db->skill->name); break;
                default: status = panic("invalid column"); break;
            }
            skill_db->index++;
            break;
    }

    return status;
}

int mob_db_create(struct mob_db * mob_db, size_t size, struct heap * heap) {
    int status = 0;

    if(map_create(&mob_db->map_id, long_compare, heap->map_pool)) {
        status = panic("failed to create map object");
    } else {
        if(store_create(&mob_db->store, size)) {
            status = panic("failed to create store object");
        } else {
            mob_db->mob = NULL;
            mob_db->index = 0;
        }
        if(status)
            map_destroy(&mob_db->map_id);
    }

    return status;
}

void mob_db_destroy(struct mob_db * mob_db) {
    store_destroy(&mob_db->store);
    map_destroy(&mob_db->map_id);
}

void mob_db_clear(struct mob_db * mob_db) {
    mob_db->index = 0;
    mob_db->mob = NULL;
    store_clear(&mob_db->store);
    map_clear(&mob_db->map_id);
}

int mob_db_parse(enum parser_event event, int mark, struct string * string, void * context) {
    int status = 0;
    struct mob_db * mob_db = context;

    switch(mark) {
        case 1:
            if(event == start) {
                mob_db->mob = store_object(&mob_db->store, sizeof(*mob_db->mob));
                if(!mob_db->mob) {
                    status = panic("failed to object store object");
                } else {
                    mob_db->index = 0;
                }
            } else if(event == end) {
                if(mob_db->index != 57) {
                    status = panic("invalid column");
                } else if(map_insert(&mob_db->map_id, &mob_db->mob->id, mob_db->mob)) {
                    status = panic("failed to insert map object");
                }
            }
            break;
        case 2:
            switch(mob_db->index) {
                case 0: status = string_strtol(string, 10, &mob_db->mob->id); break;
                case 1: status = string_store(string, &mob_db->store, &mob_db->mob->sprite); break;
                case 2: status = string_store(string, &mob_db->store, &mob_db->mob->kro); break;
                case 3: status = string_store(string, &mob_db->store, &mob_db->mob->iro); break;
                case 4: status = string_strtol(string, 10, &mob_db->mob->level); break;
                case 5: status = string_strtol(string, 10, &mob_db->mob->hp); break;
                case 6: status = string_strtol(string, 10, &mob_db->mob->sp); break;
                case 7: status = string_strtol(string, 10, &mob_db->mob->exp); break;
                case 8: status = string_strtol(string, 10, &mob_db->mob->jexp); break;
                case 9: status = string_strtol(string, 10, &mob_db->mob->range1); break;
                case 10: status = string_strtol(string, 10, &mob_db->mob->atk1); break;
                case 11: status = string_strtol(string, 10, &mob_db->mob->atk2); break;
                case 12: status = string_strtol(string, 10, &mob_db->mob->def); break;
                case 13: status = string_strtol(string, 10, &mob_db->mob->mdef); break;
                case 14: status = string_strtol(string, 10, &mob_db->mob->str); break;
                case 15: status = string_strtol(string, 10, &mob_db->mob->agi); break;
                case 16: status = string_strtol(string, 10, &mob_db->mob->vit); break;
                case 17: status = string_strtol(string, 10, &mob_db->mob->inte); break;
                case 18: status = string_strtol(string, 10, &mob_db->mob->dex); break;
                case 19: status = string_strtol(string, 10, &mob_db->mob->luk); break;
                case 20: status = string_strtol(string, 10, &mob_db->mob->range2); break;
                case 21: status = string_strtol(string, 10, &mob_db->mob->range3); break;
                case 22: status = string_strtol(string, 10, &mob_db->mob->scale); break;
                case 23: status = string_strtol(string, 10, &mob_db->mob->race); break;
                case 24: status = string_strtol(string, 10, &mob_db->mob->element); break;
                case 25: status = string_strtol(string, 16, &mob_db->mob->mode); break;
                case 26: status = string_strtol(string, 10, &mob_db->mob->speed); break;
                case 27: status = string_strtol(string, 10, &mob_db->mob->adelay); break;
                case 28: status = string_strtol(string, 10, &mob_db->mob->amotion); break;
                case 29: status = string_strtol(string, 10, &mob_db->mob->dmotion); break;
                case 30: status = string_strtod(string, &mob_db->mob->mexp); break;
                case 31: status = string_strtol(string, 10, &mob_db->mob->mvp_drop_id[0]); break;
                case 32: status = string_strtol(string, 10, &mob_db->mob->mvp_drop_chance[0]); break;
                case 33: status = string_strtol(string, 10, &mob_db->mob->mvp_drop_id[1]); break;
                case 34: status = string_strtol(string, 10, &mob_db->mob->mvp_drop_chance[1]); break;
                case 35: status = string_strtol(string, 10, &mob_db->mob->mvp_drop_id[2]); break;
                case 36: status = string_strtol(string, 10, &mob_db->mob->mvp_drop_chance[2]); break;
                case 37: status = string_strtol(string, 10, &mob_db->mob->drop_id[0]); break;
                case 38: status = string_strtol(string, 10, &mob_db->mob->drop_chance[0]); break;
                case 39: status = string_strtol(string, 10, &mob_db->mob->drop_id[1]); break;
                case 40: status = string_strtol(string, 10, &mob_db->mob->drop_chance[1]); break;
                case 41: status = string_strtol(string, 10, &mob_db->mob->drop_id[2]); break;
                case 42: status = string_strtol(string, 10, &mob_db->mob->drop_chance[2]); break;
                case 43: status = string_strtol(string, 10, &mob_db->mob->drop_id[3]); break;
                case 44: status = string_strtol(string, 10, &mob_db->mob->drop_chance[3]); break;
                case 45: status = string_strtol(string, 10, &mob_db->mob->drop_id[4]); break;
                case 46: status = string_strtol(string, 10, &mob_db->mob->drop_chance[4]); break;
                case 47: status = string_strtol(string, 10, &mob_db->mob->drop_id[5]); break;
                case 48: status = string_strtol(string, 10, &mob_db->mob->drop_chance[5]); break;
                case 49: status = string_strtol(string, 10, &mob_db->mob->drop_id[6]); break;
                case 50: status = string_strtol(string, 10, &mob_db->mob->drop_chance[6]); break;
                case 51: status = string_strtol(string, 10, &mob_db->mob->drop_id[7]); break;
                case 52: status = string_strtol(string, 10, &mob_db->mob->drop_chance[7]); break;
                case 53: status = string_strtol(string, 10, &mob_db->mob->drop_id[8]); break;
                case 54: status = string_strtol(string, 10, &mob_db->mob->drop_chance[8]); break;
                case 55: status = string_strtol(string, 10, &mob_db->mob->drop_card_id); break;
                case 56: status = string_strtol(string, 10, &mob_db->mob->drop_card_chance); break;
                default: status = panic("invalid column"); break;
            }
            mob_db->index++;
            break;
    }

    return status;
}

int mob_race_db_create(struct mob_race_db * mob_race_db, size_t size, struct heap * heap) {
    int status = 0;

    if(map_create(&mob_race_db->map_race, (map_compare_cb) strcmp, heap->map_pool)) {
        status = panic("failed to create map object");
    } else {
        if(store_create(&mob_race_db->store, size)) {
            status = panic("failed to create store object");
        } else {
            if(strbuf_create(&mob_race_db->strbuf, size)) {
                status = panic("failed to create strbuf object");
            } else {
                mob_race_db->mob_race = NULL;
                mob_race_db->index = 0;
            }
            if(status)
                store_destroy(&mob_race_db->store);
        }
        if(status)
            map_destroy(&mob_race_db->map_race);
    }

    return status;
}

void mob_race_db_destroy(struct mob_race_db * mob_race_db) {
    strbuf_destroy(&mob_race_db->strbuf);
    store_destroy(&mob_race_db->store);
    map_destroy(&mob_race_db->map_race);
}

void mob_race_db_clear(struct mob_race_db * mob_race_db) {
    mob_race_db->index = 0;
    mob_race_db->mob_race = NULL;
    strbuf_clear(&mob_race_db->strbuf);
    store_clear(&mob_race_db->store);
    map_clear(&mob_race_db->map_race);
}

int mob_race_db_parse(enum parser_event event, int mark, struct string * string, void * context) {
    int status = 0;
    struct mob_race_db * mob_race_db = context;

    struct string * id;

    switch(mark) {
        case 1:
            if(event == start) {
                mob_race_db->mob_race = store_object(&mob_race_db->store, sizeof(*mob_race_db->mob_race));
                if(!mob_race_db->mob_race) {
                    status = panic("failed to object store object");
                } else {
                    mob_race_db->index = 0;
                }
            } else if(event == end) {
                id = strbuf_string(&mob_race_db->strbuf);
                if(!id) {
                    status = panic("failed to string strbuf object");
                } else {
                    if(string_strtol_split(id, 10, ',', &mob_race_db->store, &mob_race_db->mob_race->id)) {
                        status = panic("failed to strtol split string object");
                    } else if(map_insert(&mob_race_db->map_race, mob_race_db->mob_race->race->string, mob_race_db->mob_race)) {
                        status = panic("failed to insert map object");
                    }
                }
                strbuf_clear(&mob_race_db->strbuf);
            }
            break;
        case 2:
            switch(mob_race_db->index) {
                case 0: status = string_store(string, &mob_race_db->store, &mob_race_db->mob_race->race); break;
                default: status = strbuf_strcpy(&mob_race_db->strbuf, string->string, string->length) || strbuf_putc(&mob_race_db->strbuf, ','); break;
            }
            mob_race_db->index++;
            break;
    }

    return status;
}

int mercenary_db_create(struct mercenary_db * mercenary_db, size_t size, struct heap * heap) {
    int status = 0;

    if(map_create(&mercenary_db->map_id, long_compare, heap->map_pool)) {
        status = panic("failed to create map object");
    } else {
        if(store_create(&mercenary_db->store, size)) {
            status = panic("failed to create store object");
        } else {
            mercenary_db->mercenary = NULL;
            mercenary_db->index = 0;
        }
        if(status)
            map_destroy(&mercenary_db->map_id);
    }

    return status;
}

void mercenary_db_destroy(struct mercenary_db * mercenary_db) {
    store_destroy(&mercenary_db->store);
    map_destroy(&mercenary_db->map_id);
}

void mercenary_db_clear(struct mercenary_db * mercenary_db) {
    mercenary_db->index = 0;
    mercenary_db->mercenary = NULL;
    store_clear(&mercenary_db->store);
    map_clear(&mercenary_db->map_id);
}

int mercenary_db_parse(enum parser_event event, int mark, struct string * string, void * context) {
    int status = 0;
    struct mercenary_db * mercenary_db = context;

    switch(mark) {
        case 1:
            if(event == start) {
                mercenary_db->mercenary = store_object(&mercenary_db->store, sizeof(*mercenary_db->mercenary));
                if(!mercenary_db->mercenary) {
                    status = panic("failed to object store object");
                } else {
                    mercenary_db->index = 0;
                }
            } else if(event == end) {
                if(mercenary_db->index != 26) {
                    status = panic("invalid column");
                } else if(map_insert(&mercenary_db->map_id, &mercenary_db->mercenary->id, mercenary_db->mercenary)) {
                    status = panic("failed to insert map object");
                }
            }
            break;
        case 2:
            switch(mercenary_db->index) {
                case 0: status = string_strtol(string, 10, &mercenary_db->mercenary->id); break;
                case 1: status = string_store(string, &mercenary_db->store, &mercenary_db->mercenary->sprite); break;
                case 2: status = string_store(string, &mercenary_db->store, &mercenary_db->mercenary->name); break;
                case 3: status = string_strtol(string, 10, &mercenary_db->mercenary->level); break;
                case 4: status = string_strtol(string, 10, &mercenary_db->mercenary->hp); break;
                case 5: status = string_strtol(string, 10, &mercenary_db->mercenary->sp); break;
                case 6: status = string_strtol(string, 10, &mercenary_db->mercenary->range1); break;
                case 7: status = string_strtol(string, 10, &mercenary_db->mercenary->atk1); break;
                case 8: status = string_strtol(string, 10, &mercenary_db->mercenary->atk2); break;
                case 9: status = string_strtol(string, 10, &mercenary_db->mercenary->def); break;
                case 10: status = string_strtol(string, 10, &mercenary_db->mercenary->mdef); break;
                case 11: status = string_strtol(string, 10, &mercenary_db->mercenary->str); break;
                case 12: status = string_strtol(string, 10, &mercenary_db->mercenary->agi); break;
                case 13: status = string_strtol(string, 10, &mercenary_db->mercenary->vit); break;
                case 14: status = string_strtol(string, 10, &mercenary_db->mercenary->ini); break;
                case 15: status = string_strtol(string, 10, &mercenary_db->mercenary->dex); break;
                case 16: status = string_strtol(string, 10, &mercenary_db->mercenary->luk); break;
                case 17: status = string_strtol(string, 10, &mercenary_db->mercenary->range2); break;
                case 18: status = string_strtol(string, 10, &mercenary_db->mercenary->range3); break;
                case 19: status = string_strtol(string, 10, &mercenary_db->mercenary->scale); break;
                case 20: status = string_strtol(string, 10, &mercenary_db->mercenary->race); break;
                case 21: status = string_strtol(string, 10, &mercenary_db->mercenary->element); break;
                case 22: status = string_strtol(string, 10, &mercenary_db->mercenary->speed); break;
                case 23: status = string_strtol(string, 10, &mercenary_db->mercenary->adelay); break;
                case 24: status = string_strtol(string, 10, &mercenary_db->mercenary->amotion); break;
                case 25: status = string_strtol(string, 10, &mercenary_db->mercenary->dmotion); break;
                default: status = panic("invalid column"); break;
            }
            mercenary_db->index++;
            break;
    }

    return status;
}

int produce_db_create(struct produce_db * produce_db, size_t size, struct heap * heap) {
    int status = 0;

    if(map_create(&produce_db->map_id, long_compare, heap->map_pool)) {
        status = panic("failed to create map object");
    } else {
        if(store_create(&produce_db->store, size)) {
            status = panic("failed to create store object");
        } else {
            if(strbuf_create(&produce_db->strbuf, size)) {
                status = panic("failed to create strbuf object");
            } else {
                produce_db->produce = NULL;
                produce_db->index = 0;
            }
            if(status)
                store_destroy(&produce_db->store);
        }
        if(status)
            map_destroy(&produce_db->map_id);
    }

    return status;
}

void produce_db_destroy(struct produce_db * produce_db) {
    strbuf_destroy(&produce_db->strbuf);
    store_destroy(&produce_db->store);
    map_destroy(&produce_db->map_id);
}

void produce_db_clear(struct produce_db * produce_db) {
    produce_db->index = 0;
    produce_db->produce = NULL;
    strbuf_clear(&produce_db->strbuf);
    store_clear(&produce_db->store);
    map_clear(&produce_db->map_id);
}

int produce_db_parse(enum parser_event event, int mark, struct string * string, void * context) {
    int status = 0;
    struct produce_db * produce_db = context;

    struct string * material;

    switch(mark) {
        case 1:
            if(event == start) {
                produce_db->produce = store_object(&produce_db->store, sizeof(*produce_db->produce));
                if(!produce_db->produce) {
                    status = panic("failed to object store object");
                } else {
                    produce_db->index = 0;
                }
            } else if(event == end) {
                material = strbuf_string(&produce_db->strbuf);
                if(!material) {
                    status = panic("failed to string strbuf object");
                } else {
                    if(string_strtol_split(material, 10, ',', &produce_db->store, &produce_db->produce->material)) {
                        status = panic("failed to strtol split string object");
                    } else if(map_insert(&produce_db->map_id, &produce_db->produce->id, produce_db->produce)) {
                        status = panic("failed to insert map object");
                    }
                }
                strbuf_clear(&produce_db->strbuf);
            }
            break;
        case 2:
            switch(produce_db->index) {
                case 0: status = string_strtol(string, 10, &produce_db->produce->id); break;
                case 1: status = string_strtol(string, 10, &produce_db->produce->item_id); break;
                case 2: status = string_strtol(string, 10, &produce_db->produce->item_lv); break;
                case 3: status = string_strtol(string, 10, &produce_db->produce->skill_id); break;
                case 4: status = string_strtol(string, 10, &produce_db->produce->skill_lv); break;
                default: status = strbuf_strcpy(&produce_db->strbuf, string->string, string->length) || strbuf_putc(&produce_db->strbuf, ','); break;
            }
            produce_db->index++;
            break;
    }

    return status;
}

int constant_db_create(struct constant_db * constant_db, size_t size, struct heap * heap) {
    if(map_create(&constant_db->map_macro, (map_compare_cb) strcmp, heap->map_pool))
        goto e0;
    if(map_create(&constant_db->vip_status, (map_compare_cb) strcmp, heap->map_pool))
        goto e1;
    if(map_create(&constant_db->elements, (map_compare_cb) strcmp, heap->map_pool))
        goto e2;
    if(map_create(&constant_db->jobs, (map_compare_cb) strcmp, heap->map_pool))
        goto e3;
    if(map_create(&constant_db->sc_end, (map_compare_cb) strcmp, heap->map_pool))
        goto e4;
    if(map_create(&constant_db->sizes, (map_compare_cb) strcmp, heap->map_pool))
        goto e5;
    if(map_create(&constant_db->announces, (map_compare_cb) strcmp, heap->map_pool))
        goto e6;
    if(map_create(&constant_db->sc_start, (map_compare_cb) strcmp, heap->map_pool))
        goto e7;
    if(map_create(&constant_db->locations, (map_compare_cb) strcmp, heap->map_pool))
        goto e8;
    if(map_create(&constant_db->readparam, (map_compare_cb) strcmp, heap->map_pool))
        goto e9;
    if(map_create(&constant_db->itemgroups, (map_compare_cb) strcmp, heap->map_pool))
        goto e10;
    if(map_create(&constant_db->races, (map_compare_cb) strcmp, heap->map_pool))
        goto e11;
    if(map_create(&constant_db->effects, (map_compare_cb) strcmp, heap->map_pool))
        goto e12;
    if(map_create(&constant_db->gettimes, (map_compare_cb) strcmp, heap->map_pool))
        goto e13;
    if(map_create(&constant_db->classes, (map_compare_cb) strcmp, heap->map_pool))
        goto e14;
    if(map_create(&constant_db->options, (map_compare_cb) strcmp, heap->map_pool))
        goto e15;
    if(map_create(&constant_db->mapflags, (map_compare_cb) strcmp, heap->map_pool))
        goto e16;
    if(store_create(&constant_db->store, size))
        goto e17;
    if(strbuf_create(&constant_db->strbuf, size))
        goto e18;
    constant_db->constant = NULL;
    return 0;

e18:store_destroy(&constant_db->store);
e17:map_destroy(&constant_db->mapflags);
e16:map_destroy(&constant_db->options);
e15:map_destroy(&constant_db->classes);
e14:map_destroy(&constant_db->gettimes);
e13:map_destroy(&constant_db->effects);
e12:map_destroy(&constant_db->races);
e11:map_destroy(&constant_db->itemgroups);
e10:map_destroy(&constant_db->readparam);
e9: map_destroy(&constant_db->locations);
e8: map_destroy(&constant_db->sc_start);
e7: map_destroy(&constant_db->announces);
e6: map_destroy(&constant_db->sizes);
e5: map_destroy(&constant_db->sc_end);
e4: map_destroy(&constant_db->jobs);
e3: map_destroy(&constant_db->elements);
e2: map_destroy(&constant_db->vip_status);
e1: map_destroy(&constant_db->map_macro);
e0: return 1;
}

void constant_db_destroy(struct constant_db * constant_db) {
    strbuf_destroy(&constant_db->strbuf);
    store_destroy(&constant_db->store);
    map_destroy(&constant_db->mapflags);
    map_destroy(&constant_db->options);
    map_destroy(&constant_db->classes);
    map_destroy(&constant_db->gettimes);
    map_destroy(&constant_db->effects);
    map_destroy(&constant_db->races);
    map_destroy(&constant_db->itemgroups);
    map_destroy(&constant_db->readparam);
    map_destroy(&constant_db->locations);
    map_destroy(&constant_db->sc_start);
    map_destroy(&constant_db->announces);
    map_destroy(&constant_db->sizes);
    map_destroy(&constant_db->sc_end);
    map_destroy(&constant_db->jobs);
    map_destroy(&constant_db->elements);
    map_destroy(&constant_db->vip_status);
    map_destroy(&constant_db->map_macro);
}

void constant_db_clear(struct constant_db * constant_db) {
    constant_db->constant = NULL;
    strbuf_clear(&constant_db->strbuf);
    store_clear(&constant_db->store);
    map_clear(&constant_db->mapflags);
    map_clear(&constant_db->options);
    map_clear(&constant_db->classes);
    map_clear(&constant_db->gettimes);
    map_clear(&constant_db->effects);
    map_clear(&constant_db->races);
    map_clear(&constant_db->itemgroups);
    map_clear(&constant_db->readparam);
    map_clear(&constant_db->locations);
    map_clear(&constant_db->sc_start);
    map_clear(&constant_db->announces);
    map_clear(&constant_db->sizes);
    map_clear(&constant_db->sc_end);
    map_clear(&constant_db->jobs);
    map_clear(&constant_db->elements);
    map_clear(&constant_db->vip_status);
    map_clear(&constant_db->map_macro);
}

int constant_db_parse(enum parser_event event, int mark, struct string * string, void * context) {
    int status = 0;
    struct constant_db * constant_db = context;

    struct string * range;

    switch(mark) {
        case 2:
            if(event == start) {
                constant_db->constant = store_object(&constant_db->store, sizeof(*constant_db->constant));
                if(!constant_db->constant)
                    status = panic("failed to object store object");
            } else if(event == end) {
                if(!constant_db->constant->macro) {
                    status = panic("invalid string object");
                } else if(map_insert(&constant_db->map_macro, constant_db->constant->macro->string, constant_db->constant)) {
                    status = panic("failed to insert map object");
                }
            }
            break;
        case 3: status = string_store(string, &constant_db->store, &constant_db->constant->macro); break;
        case 4: status = string_strtol(string, 10, &constant_db->constant->value); break;
        case 5: status = string_store(string, &constant_db->store, &constant_db->constant->label); break;
        case 6:
            if(event == end) {
                range = strbuf_string(&constant_db->strbuf);
                if(!range) {
                    status = panic("failed to string strbuf object");
                } else if(string_strtol_split(range, 10, ',', &constant_db->store, &constant_db->constant->range)) {
                    status = panic("failed to strtol split string object");
                }
                strbuf_clear(&constant_db->strbuf);
            }
            break;
        case 7: status = strbuf_strcpy(&constant_db->strbuf, string->string, string->length) || strbuf_putc(&constant_db->strbuf, ','); break;
    }

    return status;
}

int constant_group_parse(enum parser_event event, int mark, struct string * string, void * context) {
    int status = 0;
    struct constant_db * constant_db = context;

    struct constant * constant;

    if(string) {
        constant = map_search(&constant_db->map_macro, string->string);
        if(!constant) {
            status = panic("invalid macro - %s", string->string);
        } else {
            switch(mark) {
                case 2: status = map_insert(&constant_db->vip_status, constant->macro->string, constant); break;
                case 4: status = map_insert(&constant_db->elements, constant->macro->string, constant); break;
                case 6: status = map_insert(&constant_db->jobs, constant->macro->string, constant); break;
                case 8: status = map_insert(&constant_db->sc_end, constant->macro->string, constant); break;
                case 10: status = map_insert(&constant_db->sizes, constant->macro->string, constant); break;
                case 12: status = map_insert(&constant_db->announces, constant->macro->string, constant); break;
                case 14: status = map_insert(&constant_db->sc_start, constant->macro->string, constant); break;
                case 16: status = map_insert(&constant_db->locations, constant->macro->string, constant); break;
                case 18: status = map_insert(&constant_db->readparam, constant->macro->string, constant); break;
                case 20: status = map_insert(&constant_db->itemgroups, constant->macro->string, constant); break;
                case 22: status = map_insert(&constant_db->races, constant->macro->string, constant); break;
                case 24: status = map_insert(&constant_db->effects, constant->macro->string, constant); break;
                case 26: status = map_insert(&constant_db->gettimes, constant->macro->string, constant); break;
                case 28: status = map_insert(&constant_db->classes, constant->macro->string, constant); break;
                case 30: status = map_insert(&constant_db->options, constant->macro->string, constant); break;
                case 32: status = map_insert(&constant_db->mapflags, constant->macro->string, constant); break;
            }
        }
    }

    return status;
}

int lookup_create(struct lookup * lookup, size_t size, struct heap * heap) {
    if(schema_create(&lookup->schema, heap))
        goto e0;
    if(parser_create(&lookup->parser, size, heap))
        goto e1;
    if(pet_db_create(&lookup->pet_db, size, heap))
        goto e2;
    if(item_db_create(&lookup->item_db, size, heap))
        goto e3;
    if(skill_db_create(&lookup->skill_db, size, heap))
        goto e4;
    if(mob_db_create(&lookup->mob_db, size, heap))
        goto e5;
    if(mob_race_db_create(&lookup->mob_race_db, size, heap))
        goto e6;
    if(mercenary_db_create(&lookup->mercenary_db, size, heap))
        goto e7;
    if(produce_db_create(&lookup->produce_db, size, heap))
        goto e8;
    if(constant_db_create(&lookup->constant_db, size, heap))
        goto e9;
    return 0;

e9: produce_db_destroy(&lookup->produce_db);
e8: mercenary_db_destroy(&lookup->mercenary_db);
e7: mob_race_db_destroy(&lookup->mob_race_db);
e6: mob_db_destroy(&lookup->mob_db);
e5: skill_db_destroy(&lookup->skill_db);
e4: item_db_destroy(&lookup->item_db);
e3: pet_db_destroy(&lookup->pet_db);
e2: parser_destroy(&lookup->parser);
e1: schema_destroy(&lookup->schema);
e0: return 1;
}

void lookup_destroy(struct lookup * lookup) {
    constant_db_destroy(&lookup->constant_db);
    produce_db_destroy(&lookup->produce_db);
    mercenary_db_destroy(&lookup->mercenary_db);
    mob_race_db_destroy(&lookup->mob_race_db);
    mob_db_destroy(&lookup->mob_db);
    skill_db_destroy(&lookup->skill_db);
    item_db_destroy(&lookup->item_db);
    pet_db_destroy(&lookup->pet_db);
    parser_destroy(&lookup->parser);
    schema_destroy(&lookup->schema);
}

int lookup_pet_db_parse(struct lookup * lookup, char * path) {
    int status = 0;
    struct schema_node * pet_db_schema;

    pet_db_clear(&lookup->pet_db);

    pet_db_schema = schema_load(&lookup->schema, pet_db_markup);
    if(!pet_db_schema) {
        status = panic("failed to load schema object");
    } else if(parser_parse(&lookup->parser, path, pet_db_schema, pet_db_parse, &lookup->pet_db)) {
        status = panic("failed to parse parser object");
    }

    return status;
}

int lookup_item_db_parse(struct lookup * lookup, char * path) {
    int status = 0;
    struct schema_node * item_db_schema;

    item_db_clear(&lookup->item_db);

    item_db_schema = schema_load(&lookup->schema, csv_markup);
    if(!item_db_schema) {
        status = panic("failed to load schema object");
    } else if(parser_parse(&lookup->parser, path, item_db_schema, item_db_parse, &lookup->item_db)) {
        status = panic("failed to parse parser object");
    }

    return status;
}

int lookup_item_combo_db_parse(struct lookup * lookup, char * path) {
    int status = 0;
    struct schema_node * item_combo_db_schema;

    item_combo_db_schema = schema_load(&lookup->schema, csv_markup);
    if(!item_combo_db_schema) {
        status = panic("failed to load schema object");
    } else if(parser_parse(&lookup->parser, path, item_combo_db_schema, item_combo_db_parse, &lookup->item_db)) {
        status = panic("failed to parse parser object");
    }

    return status;
}

int lookup_skill_db_parse(struct lookup * lookup, char * path) {
    int status = 0;
    struct schema_node * skill_db_schema;

    skill_db_clear(&lookup->skill_db);

    skill_db_schema = schema_load(&lookup->schema, csv_markup);
    if(!skill_db_schema) {
        status = panic("failed to load schema object");
    } else if(parser_parse(&lookup->parser, path, skill_db_schema, skill_db_parse, &lookup->skill_db)) {
        status = panic("failed to parse parser object");
    }

    return status;
}

int lookup_mob_db_parse(struct lookup * lookup, char * path) {
    int status = 0;
    struct schema_node * mob_db_schema;

    mob_db_clear(&lookup->mob_db);

    mob_db_schema = schema_load(&lookup->schema, csv_markup);
    if(!mob_db_schema) {
        status = panic("failed to load schema object");
    } else if(parser_parse(&lookup->parser, path, mob_db_schema, mob_db_parse, &lookup->mob_db)) {
        status = panic("failed to parse parser object");
    }

    return status;
}

int lookup_mob_race_db_parse(struct lookup * lookup, char * path) {
    int status = 0;
    struct schema_node * mob_race_schema;

    mob_race_db_clear(&lookup->mob_race_db);

    mob_race_schema = schema_load(&lookup->schema, csv_markup);
    if(!mob_race_schema) {
        status = panic("failed to load schema object");
    } else if(parser_parse(&lookup->parser, path, mob_race_schema, mob_race_db_parse, &lookup->mob_race_db)) {
        status = panic("failed to parse parser object");
    }

    return status;
}

int lookup_mercenary_db_parse(struct lookup * lookup, char * path) {
    int status = 0;
    struct schema_node * mercenary_db_schema;

    mercenary_db_clear(&lookup->mercenary_db);

    mercenary_db_schema = schema_load(&lookup->schema, csv_markup);
    if(!mercenary_db_schema) {
        status = panic("failed to load schema object");
    } else if(parser_parse(&lookup->parser, path, mercenary_db_schema, mercenary_db_parse, &lookup->mercenary_db)) {
        status = panic("failed to parse parser object");
    }

    return status;
}

int lookup_produce_db_parse(struct lookup * lookup, char * path) {
    int status = 0;
    struct schema_node * produce_db_schema;

    produce_db_clear(&lookup->produce_db);

    produce_db_schema = schema_load(&lookup->schema, csv_markup);
    if(!produce_db_schema) {
        status = panic("failed to load schema object");
    } else if(parser_parse(&lookup->parser, path, produce_db_schema, produce_db_parse, &lookup->produce_db)) {
        status = panic("failed to parse parser object");
    }

    return status;
}

int lookup_constant_db_parse(struct lookup * lookup, char * path) {
    int status = 0;
    struct schema_node * constant_db_schema;

    constant_db_clear(&lookup->constant_db);

    constant_db_schema = schema_load(&lookup->schema, constant_db_markup);
    if(!constant_db_schema) {
        status = panic("failed to load schema object");
    } else if(parser_parse(&lookup->parser, path, constant_db_schema, constant_db_parse, &lookup->constant_db)) {
        status = panic("failed to parse parser object");
    }

    return status;
}

int lookup_constant_group_parse(struct lookup * lookup, char * path) {
    int status = 0;
    struct schema_node * constant_group_schema;

    constant_group_schema = schema_load(&lookup->schema, constant_group_markup);
    if(!constant_group_schema) {
        status = panic("failed to load schema object");
    } else if(parser_parse(&lookup->parser, path, constant_group_schema, constant_group_parse, &lookup->constant_db)) {
        status = panic("failed to parse parser object");
    }

    return status;
}
