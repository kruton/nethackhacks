#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <cstdlib>

using namespace std;

vector<unsigned short> NameIndices;
vector<unsigned short> AppearanceIndices;

#define MAX_INDEX 360
int default_names_to_appearances[MAX_INDEX] = {-1};

class Object
{
    public:
        string Name;
        string Appearance;
        int Index;
        int AppearanceIndex;
        int NameIndex;

        Object(string name, string appearance, int index) : Name(name), Appearance(appearance), Index(index)
        { AppearanceIndex = -1; NameIndex = -1; }
};

class ConfigurationItem
{
    public:
        vector<Object>* ObjectVector;
        int NameIndex;
        int AppearanceIndex;

        ConfigurationItem(vector<Object>* o, int n, int a) :
        ObjectVector(o), NameIndex(n), AppearanceIndex(a) { }
};

Object RingList[] = { Object("adornment", "wooden", 150),
                      Object("gain strength", "granite", 151),
                      Object("gain constitution", "opal", 152),
                      Object("increase accuracy", "clay", 153),
                      Object("increase damage", "coral", 154),
                      Object("protection", "black onyx", 155),
                      Object("regeneration", "moonstone", 156),
                      Object("searching", "tiger eye", 157),
                      Object("stealth", "jade", 158),
                      Object("sustain ability", "bronze", 159),
                      Object("levitation", "agate", 160),
                      Object("hunger", "topaz", 161),
                      Object("aggravate monster", "sapphire", 162),
                      Object("conflict", "ruby", 163),
                      Object("warning", "diamond", 164),
                      Object("poison resistance", "pearl", 165),
                      Object("fire resistance", "iron", 166),
                      Object("cold resistance", "brass", 167),
                      Object("shock resistance", "copper", 168),
                      Object("free action", "twisted", 169),
                      Object("slow digestion", "steel", 170),
                      Object("teleportation", "silver", 171),
                      Object("teleport control", "gold", 172),
                      Object("polymorph", "ivory", 173),
                      Object("polymorph control", "emerald", 174),
                      Object("invisibility", "wire", 175),
                      Object("see invisible", "engagement", 176),
                      Object("protection from shape changers", "shiny", 177)
                    };
const int RingListSize = 28;

Object AmuletList[] = { Object("amulet of ESP", "circular", 178),
                        Object("amulet of life saving", "spherical", 179),
                        Object("amulet of strangulation", "oval", 180),
                        Object("amulet of restful sleep", "triangular", 181),
                        Object("amulet versus poison", "pyramidal", 182),
                        Object("amulet of change", "square", 183),
                        Object("amulet of unchanging",  "concave", 184),
                        Object("amulet of reflection", "hexagonal", 185),
                        Object("amulet of magical breathing", "octagonal", 186)
                      };
const int AmuletListSize = 9;

Object PotionList[] = { Object("gain ability", "ruby", 268),
                        Object("restore ability", "pink", 269),
                        Object("confusion", "orange", 270),
                        Object("blindness", "yellow", 271),
                        Object("paralysis", "emerald", 272),
                        Object("speed", "dark green", 273),
                        Object("levitation", "cyan", 274),
                        Object("hallucination", "sky blue", 275),
                        Object("invisibility", "brilliant blue", 276),
                        Object("see invisible", "magenta", 277),
                        Object("healing", "purple-red", 278),
                        Object("extra healing", "puce", 279),
                        Object("gain level", "milky", 280),
                        Object("enlightenment", "swirly", 281),
                        Object("monster detection", "bubbly", 282),
                        Object("object detection", "smoky", 283),
                        Object("gain energy", "cloudy", 284),
                        Object("sleeping", "effervescent", 285),
                        Object("full healing", "black", 286),
                        Object("polymorph", "golden", 287),
                        Object("booze", "brown", 288),
                        Object("sickness", "fizzy", 289),
                        Object("fruit juice", "dark", 290),
                        Object("acid", "white", 291),
                        Object("oil", "murky", 292)
                      };
const int PotionListSize = 25;

Object ScrollList[] = { Object("enchant armor", "ZELGO MER", 294),
                        Object("destroy armor", "JUYED AWK YACC", 295),
                        Object("confuse monster", "NR 9", 296),
                        Object("scare monster", "XIXAXA XOXAXA XUXAXA", 297),
                        Object("remove curse", "PRATYAVAYAH", 298),
                        Object("enchant weapon", "DAIYEN FOOELS", 299),
                        Object("create monster", "LEP GEX VEN ZEA", 300),
                        Object("taming", "PRIRUTSENIE", 301),
                        Object("genocide", "ELBIB YLOH", 302),
                        Object("light", "VERR YED HORRE", 303),
                        Object("teleportation", "VENZAR BORGAVVE", 304),
                        Object("gold detection", "THARR", 305),
                        Object("food detection", "YUM YUM", 306),
                        Object("identify", "KERNOD WEL", 307),
                        Object("magic mapping", "ELAM EBOW", 308),
                        Object("amnesia", "DUAM XNAHT", 309),
                        Object("fire", "ANDOVA BEGARIN", 310),
                        Object("earth", "KIRJE", 311),
                        Object("punishment", "VE FORBRYDERNE", 312),
                        Object("charging", "HACKEM MUCHE", 313),
                        Object("stinking cloud", "VELOX NEB", 314),
                        Object("(unused1)", "FOOBIE BLETCH", 315),
                        Object("(unused2)", "TEMOV", 316),
                        Object("(unused3)", "GARVEN DEH", 317),
                        Object("(unused4)", "READ ME", 318)
                      };
static int ScrollListSize = 25;

Object SpellbookList[] = { Object("dig", "parchment", 321),
                           Object("magic missile", "vellum", 322),
                           Object("fireball", "ragged", 323),
                           Object("cone of cold", "dog eared", 324),
                           Object("sleep", "mottled", 325),
                           Object("finger of death", "stained", 326),
                           Object("light", "cloth", 327),
                           Object("detect monsters", "leather", 328),
                           Object("healing", "white", 329),
                           Object("knock", "pink", 330),
                           Object("force bolt", "red", 331),
                           Object("confuse monster", "orange", 332),
                           Object("cure blindness", "yellow", 333),
                           Object("drain life", "velvet", 334),
                           Object("slow monster", "light green", 335),
                           Object("wizard lock", "dark green", 336),
                           Object("create monster", "turquoise", 337),
                           Object("detect food", "cyan", 338),
                           Object("cause fear", "light blue", 339),
                           Object("clairvoyance", "dark blue", 340),
                           Object("cure sickness", "indigo", 341),
                           Object("charm monster", "magenta", 342),
                           Object("haste self", "purple", 343),
                           Object("detect unseen", "violet", 344),
                           Object("levitation", "tan", 345),
                           Object("extra healing", "plaid", 346),
                           Object("restore ability", "light brown", 347),
                           Object("invisibility", "dark brown", 348),
                           Object("detect treasure", "gray", 349),
                           Object("remove curse", "wrinkled", 350),
                           Object("magic mapping", "dusty", 351),
                           Object("identify", "bronze", 352),
                           Object("turn undead", "copper", 353),
                           Object("polymorph", "silver", 354),
                           Object("teleport away", "gold", 355),
                           Object("create familiar", "glittering", 356),
                           Object("cancellation", "shining", 357),
                           Object("protection", "dull", 358),
                           Object("jumping", "thin", 359),
                           Object("stone to flesh", "thick", 360)
                       };
const int SpellbookListSize = 40;

vector<Object> RingVector, AmuletVector, PotionVector, ScrollVector, SpellbookVector;
vector<ConfigurationItem> configuration_items;

vector<Object> buildObjectVector(Object* object_list, int list_size)
{
    vector<Object> result;
    int i1;
    for (i1 = 0; i1 < list_size; i1++)
        result.push_back(object_list[i1]);
    return result;
}

void buildAllObjectVectors()
{
    RingVector = buildObjectVector(RingList, RingListSize);
    AmuletVector = buildObjectVector(AmuletList, AmuletListSize);
    PotionVector = buildObjectVector(PotionList, PotionListSize);
    ScrollVector = buildObjectVector(ScrollList, ScrollListSize);
    SpellbookVector = buildObjectVector(SpellbookList, SpellbookListSize);
}

void buildIndices(Object* object_list, int list_size)
{
    int i1;
    for (i1 = 0; i1 < list_size; i1++)
    {
        object_list[i1].AppearanceIndex = AppearanceIndices.size();
        object_list[i1].NameIndex = NameIndices.size();
        AppearanceIndices.push_back(AppearanceIndices.size());
        NameIndices.push_back(NameIndices.size());
    }
}

void buildAllIndices()
{
    buildIndices(RingList, RingListSize);
    buildIndices(AmuletList, AmuletListSize);
    buildIndices(PotionList, PotionListSize);
    buildIndices(ScrollList, ScrollListSize);
    buildIndices(SpellbookList, SpellbookListSize);

    vector<unsigned short> copy = AppearanceIndices;
    AppearanceIndices = copy;
    copy = NameIndices;
    NameIndices = copy;

    for (unsigned int i1 = 0; i1 < AppearanceIndices.size(); i1++)
        default_names_to_appearances[NameIndices[i1]] = AppearanceIndices[i1];

}

void showAppearanceIndices(const Object* object_list, int list_size)
{
    int i1;
    for (i1 = 0; i1 < list_size; i1++)
        cout << "Appearance: " << object_list[i1].Appearance << ", index: " << object_list[i1].AppearanceIndex << endl;
}

void showAllAppearanceIndices()
{
    cout << "RINGS:" << endl;
    showAppearanceIndices(RingList, RingListSize);
    cout << "AMULETS:" << endl;
    showAppearanceIndices(AmuletList, AmuletListSize);
    cout << "POTIONS:" << endl;
    showAppearanceIndices(PotionList, PotionListSize);
    cout << "SCROLLS:" << endl;
    showAppearanceIndices(ScrollList, ScrollListSize);
    cout << "SPELLBOOKS:" << endl;
    showAppearanceIndices(SpellbookList, SpellbookListSize);
}

bool shuffle(const vector<Object> &object_vector, int *names_to_appearances)
{
    int i1, vector_size = object_vector.size();
    for (i1 = 0; i1 < vector_size; i1++)
    {
        int swap_target = i1 + (random() % (vector_size - i1));

        int tmp = names_to_appearances[object_vector[i1].NameIndex];
        names_to_appearances[object_vector[i1].NameIndex] = names_to_appearances[object_vector[swap_target].NameIndex];
        names_to_appearances[object_vector[swap_target].NameIndex] = tmp;
    }

    for (vector<ConfigurationItem>::const_iterator i = configuration_items.begin(); i != configuration_items.end(); ++i)
        if (((*i).ObjectVector) == &object_vector &&
            names_to_appearances[(*i).NameIndex] != -1 &&
            names_to_appearances[(*i).NameIndex] != (*i).AppearanceIndex)
            return false;

    return true;
}

void showAppearances(vector<Object> &object_vector, int *names_to_appearances)
{
    for (vector<Object>::const_iterator i = object_vector.begin(); i != object_vector.end(); ++i) {
        string appearance;
        for (unsigned int a = 0; a < object_vector.size(); ++a)
            if (object_vector[a].AppearanceIndex == names_to_appearances[(*i).NameIndex]) {
                appearance = object_vector[a].Appearance;
                break;
            }
        cout << "  " << (*i).Name << " = " << appearance << endl;
    }
}

void showAllAppearances(int seed, int offset)
{
    srandom(seed);
    for (int i = 0; i < offset; ++i)
        random();

    int names_to_appearances[MAX_INDEX];
    memcpy(names_to_appearances, default_names_to_appearances, sizeof(int) * MAX_INDEX);
    shuffle(RingVector, names_to_appearances);
    shuffle(AmuletVector, names_to_appearances);
    shuffle(PotionVector, names_to_appearances);
    shuffle(ScrollVector, names_to_appearances);
    shuffle(SpellbookVector, names_to_appearances);

    cout << "RINGS:" << endl;
    showAppearances(RingVector, names_to_appearances);
    cout << "AMULETS:" << endl;
    showAppearances(AmuletVector, names_to_appearances);
    cout << "POTIONS:" << endl;
    showAppearances(PotionVector, names_to_appearances);
    cout << "SCROLLS:" << endl;
    showAppearances(ScrollVector, names_to_appearances);
    cout << "SPELLBOOKS:" << endl;
    showAppearances(SpellbookVector, names_to_appearances);
}

int getVectorName(const vector<Object> &object_vector, const string &s)
{
    int i1, len = object_vector.size();
    for (i1 = 0; i1 < len; i1++)
        if (object_vector[i1].Name == s)
            return object_vector[i1].NameIndex;

    return -1;
}

int getVectorAppearanceIndex(const vector<Object> &object_vector, const string &s)
{
    int i1, len = object_vector.size();
    for (i1 = 0; i1 < len; i1++)
        if (object_vector[i1].Appearance == s)
            return object_vector[i1].AppearanceIndex;

    return -1;
}

int readConfigurationFile(const string &conf_filename, vector<ConfigurationItem> &ci)
{
    fstream conf_stream(conf_filename.c_str(), fstream::in);
    if (conf_stream.fail())
    {
        cerr << "Could not open " << conf_filename << "." << endl;
        return -1;
    }

    // Type to vector
    map<string, vector<Object>* > type_to_vector;
    type_to_vector["spellbook"] = &SpellbookVector;
    type_to_vector["ring"] = &RingVector;
    type_to_vector["scroll"] = &ScrollVector;
    type_to_vector["potion"] = &PotionVector;
    type_to_vector["amulet"] = &AmuletVector;

    // Configuration file format is:
    // itemtype "itemname" "itemappearance"
    // e.g. spellbook "force bolt" "red"

    while(!conf_stream.eof())
    {
        vector<Object>* relevant_vector;

        string type;
        getline(conf_stream, type, ' ');
        if (conf_stream.eof())
            break;

        map<string, vector<Object>* >::const_iterator i1;
        for (i1 = type_to_vector.begin(); i1 != type_to_vector.end(); i1++)
            if (type == i1->first)
                break;
        if (i1 == type_to_vector.end())
        {
            cerr << "Unknown object type \"" << type << "\"." << endl;
            return -1;
        }

        relevant_vector = i1->second;

        int c = conf_stream.get();
        if (c != '\"')
        {
            cerr << "Expected \" after " << type << "." << endl;
            return -1;
        }

        string name;
        getline(conf_stream, name, '\"');
        if (conf_stream.eof())
        {
            cerr << "Unexpected end in configuration file." << endl;
            return -1;
        }

        int n_index = getVectorName(*relevant_vector, name);
        if (n_index == -1)
        {
            cerr << "No such " << type << ", \"" << name << "." << endl;
            return -1;
        }

        string appearance;
        getline(conf_stream, appearance, '\"'); // the space between name and appearance
        getline(conf_stream, appearance, '\"');
        if (conf_stream.eof())
        {
            cerr << "Unexpected end in configuration file." << endl;
            return -1;
        }

        int index = getVectorAppearanceIndex(*relevant_vector, appearance);
        if (index == -1)
        {
            cerr << "No such appearance in " << type << ", \"" << appearance << "." << endl;
            return -1;
        }

        string dummy;
        // newline
        getline(conf_stream, dummy, '\n');

        ci.push_back(ConfigurationItem(relevant_vector, n_index, index));
    }

    return 0;
}

int main(int argc, char* argv[])
{
    buildAllIndices();
    buildAllObjectVectors();

    string conf_filename("nh_srandom_crusher.conf");
    string output_filename("nh_srandom_crusher.output");

    int min_rand = 6, max_rand = 6;
    int start_index = 0, end_index = 1000000;

    int i1;
    for (i1 = 1; i1 < argc; i1++)
    {
        string argument = argv[i1];

        if (argument == string("--list") || argument == string("-l"))
        {
            showAllAppearanceIndices();
            return 0;
        }
        else if (argument == string("-i") || argument == string("--inspect"))
        {
            if (i1 == argc-2)
            {
                cerr << "Expected seed and index after -i or --inspect." << endl;
                return -1;
            }
            int seed = atoi(argv[i1+1]);
            int offset = atoi(argv[i1+2]);
            showAllAppearances(seed, offset);
            return 0;
        }
        else if (argument == string("-c") || argument == string("--conf"))
        {
            if (i1 == argc-1)
            {
                cerr << "Expected file name after -c or --conf." << endl;
                return -1;
            }
            conf_filename = string(argv[i1+1]);
            i1++;
        }
        else if (argument == string("-o") || argument == string("--output"))
        {
            if (i1 == argc-1)
            {
                cerr << "Expected file name after -o or --output." << endl;
                return -1;
            }
            output_filename = string(argv[i1+1]);
            i1++;
        }
        else if (argument == string("--randmin"))
        {
            if (i1 == argc-1)
            {
                cerr << "Expected value after --randmin" << endl;
                return -1;
            }
            min_rand = atoi(argv[i1+1]);
            i1++;
        }
        else if (argument == string("--randmax"))
        {
            if (i1 == argc-1)
            {
                cerr << "Expected value after --randmax" << endl;
                return -1;
            }
            max_rand = atoi(argv[i1+1]);
            i1++;
        }
        else if (argument == string("--indexstart"))
        {
            if (i1 == argc-1)
            {
                cerr << "Expected value after --indexstart" << endl;
                return -1;
            }
            start_index = atoi(argv[i1+1]);
            i1++;
        }
        else if (argument == string("--indexend"))
        {
            if (i1 == argc-1)
            {
                cerr << "Expected value after --indexend" << endl;
                return -1;
            }
            end_index = atoi(argv[i1+1]);
            i1++;
        }
        else if (argument == string("-h") || argument == string("--help"))
        {
            cout << "nh_srandom_crusher --conf filename" << endl;
            cout << "nh_srandom_crusher -c filename" << endl;
            cout << "    Use your own configuration file. Defaults to nh_srandom_crusher.conf." << endl;
            cout << "nh_srandom_crusher --output filename" << endl;
            cout << "nh_srandom_crusher -o filename" << endl;
            cout << "    Output candidates in a file. Defaults to nh_srandom_crusher.output." << endl;
            cout << "nh_srandom_crusher --randmin value" << endl;
            cout << "    Set the minimum value of random() calls before shuffling (default 6)." << endl;
            cout << "nh_srandom_crusher --randmax value" << endl;
            cout << "    Set the minimum value of random() calls before shuffling (default 6)." << endl;
            cout << "nh_srandom_crusher --indexstart value" << endl;
            cout << "    Start seed forcing from this index (default 0)." << endl;
            cout << "nh_srandom_crusher --indexend value" << endl;
            cout << "    Start seed forcing from this index (default 0)." << endl;
            cout << "nh_srandom_crusher --help" << endl;
            cout << "nh_srandom_crusher -h" << endl;
            cout << "    See this help" << endl;
            cout << "nh_srandom_crusher --list" << endl;
            cout << "nh_srandom_crusher -l" << endl;
            cout << "    List randomized appearance indices." << endl;
            cout << "Usage example:" << endl;
            cout << "nh_srandom_crusher -c wizard_game.conf -o wizard_game.candidates --randmax 7 --indexend 1000000000" << endl;
            cout << endl;
            cout << "The crusher takes time roughly in the form (a-b+1)*(c-d+1)*e" << endl;
            cout << "a = indexend, b = indexstart, c = randmax, d = randmin, e = number of entries in configuration file." << endl;
            cout << "e is small compared to other values." << endl;
            return 0;
        }
        else
        {
            cerr << "Unknown argument \"" << argv[i1] << "\"." << endl;
            cerr << "Use --help or -h to see accepted arguments." << endl;
            return -1;
        }
    }

    int status = readConfigurationFile(conf_filename, configuration_items);
    if (status)
        return status;

    fstream output_stream(output_filename.c_str(), fstream::out);
    if (output_stream.fail())
    {
        cerr << "Failed to open " << output_filename << " for writing." << endl;
        return -1;
    }

    int index;

    bool bailout = false;

    int cur_rand;
    int start_time0 = time(0);
    int last_time0 = start_time0;

    int names_to_appearances[MAX_INDEX];
    for (index = start_index; index <= end_index && !bailout; index++)
    {
        // Overflow protection
        if (index == 0xffffffff)
            bailout = true;

        // C as in Copy
        //vector<Object> CRingVector, CAmuletVector, CPotionVector, CScrollVector, CSpellbookVector;

        for (cur_rand = min_rand; cur_rand <= max_rand; cur_rand++)
        {
            memcpy(names_to_appearances, default_names_to_appearances, sizeof(int) * MAX_INDEX);

            srandom(index);
            for (i1 = 0; i1 < cur_rand; i1++)
                random();

            if (shuffle(RingVector, names_to_appearances) &&
                shuffle(AmuletVector, names_to_appearances) &&
                shuffle(PotionVector, names_to_appearances) &&
                shuffle(ScrollVector, names_to_appearances) &&
                shuffle(SpellbookVector, names_to_appearances))
                break;
        }

        if (cur_rand <= max_rand)
        {
            printf("New candidate: %d\n", index);
            output_stream << index << " " << cur_rand << endl;
            output_stream.flush();
        }

        if (!(index % 100000))
            if (last_time0 != time(0))
            {
                last_time0 = time(0);

                unsigned int eta = last_time0 - start_time0;
                unsigned int finished_for_now = index - start_index;
                eta = (unsigned int) (((double) (end_index - index + 1) / (double) (finished_for_now)) * (double) eta);
                printf("At seed %d, ETA %d seconds.\n", index, eta);
            }
    }

    printf("%d seeds in %ld seconds.\n", (end_index - start_index + 1), time(0) - start_time0);
    output_stream.close();

    return 0;
}

