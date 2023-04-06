#include "cyoa.hpp"
// all the functions' implementation in here
// stuff for choice
choice::choice() : currentpgNum(0), gadget(), nextpgNum(0), textMessage() {
}
choice::choice(size_t _currentpgNum,
               std::pair<std::string, long int> _gadget,
               size_t _nextpgNum,
               std::string _textMessage) :
    currentpgNum(_currentpgNum),
    gadget(_gadget),
    nextpgNum(_nextpgNum),
    textMessage(_textMessage) {
}
choice::choice(const choice & rhs) :
    currentpgNum(rhs.currentpgNum),
    gadget(rhs.gadget),
    nextpgNum(rhs.nextpgNum),
    textMessage(rhs.textMessage) {
}
choice & choice::operator=(const choice & rhs) {
  if (this != &rhs) {
    currentpgNum = rhs.currentpgNum;
    gadget = rhs.gadget;
    nextpgNum = rhs.nextpgNum;
    textMessage = rhs.textMessage;
  }
  return *this;
}
size_t choice::getcurrentpgNum() const {
  return currentpgNum;
}

size_t choice::getnextpgNum() const {
  return nextpgNum;
}

std::pair<std::string, long int> choice::getGadget() const {
  return gadget;
}

void choice::printChoice(std::map<std::string, long int> & gadgets_story) {
  // really just print the textMessage
  // bool availbale is used to indicate whether gadget is available (for step 4)
  bool available = false;
  if (gadget.first.length() == 0) {
    available = true;
  }
  else {
    available = is_ggt_match(gadgets_story, gadget);
  }
  if (available) {
    std::cout << textMessage << std::endl;
  }
  else {
    std::cout << "<UNAVAILABLE>" << std::endl;
  }
}

choice::~choice() {
}
// stuff for page
page::page() :
    pageNum(0), gadgets_pg(), pageType(Normal), pageFile(), Num_choices(0), choices() {
}
page::page(size_t _pageNum,
           std::map<std::string, long int> _gadgets_pg,
           pageTypes _pageType,
           std::string _pageFile,
           size_t _Num_choices,
           std::vector<choice> _choices) :
    pageNum(_pageNum),
    gadgets_pg(_gadgets_pg),
    pageType(_pageType),
    pageFile(_pageFile),
    Num_choices(_Num_choices),
    choices(_choices) {
  if ((pageType != Normal) && (Num_choices != 0)) {
    std::cerr << "Win and Lose pages must not have any choices!" << std::endl;
    exit(EXIT_FAILURE);
  }
}
page::page(const page & rhs) :
    pageNum(rhs.pageNum),
    gadgets_pg(rhs.gadgets_pg),
    pageType(rhs.pageType),
    pageFile(rhs.pageFile),
    Num_choices(rhs.Num_choices),
    choices(rhs.choices) {
}
page & page::operator=(const page & rhs) {
  if (this != &rhs) {
    pageNum = rhs.pageNum;
    gadgets_pg = rhs.gadgets_pg;
    pageType = rhs.pageType;
    pageFile = rhs.pageFile;
    Num_choices = rhs.Num_choices;
    choices = rhs.choices;
  }
  return *this;
}

void page::addChoice(const choice & rhs) {
  assert(rhs.getcurrentpgNum() == pageNum);
  if (pageType != Normal) {
    std::cerr << "Win and Lose pages must not have any choices!" << std::endl;
    exit(EXIT_FAILURE);
  }
  Num_choices += 1;
  choices.push_back(rhs);
}

void page::addGadget(const std::pair<std::string, long int> gadgets_pg_pair) {
  if (gadgets_pg.find(gadgets_pg_pair.first) == gadgets_pg.end()) {
    gadgets_pg.insert(gadgets_pg_pair);
  }
  else {
    gadgets_pg[gadgets_pg_pair.first] = gadgets_pg_pair.second;
  }
}

size_t page::getpageNum() const {
  return pageNum;
}

size_t page::getNumChoices() const {
  return Num_choices;
}

pageTypes page::getpageType() const {
  return pageType;
}

std::vector<choice> page::getChoices() const {
  return choices;
}

std::map<std::string, long int> page::getGadgePG() const {
  return gadgets_pg;
}

void page::printPage(bool just_print_everything,
                     std::map<std::string, long int> & gadgets_story) {
  // bool used for step 1
  if (just_print_everything) {
    std::cout << "Page " << pageNum << std::endl;
    std::cout << "==========" << std::endl;
  }
  printTxt(pageFile);
  std::map<std::string, long int> gadgets_pgg = gadgets_pg;
  if (pageType == Normal) {
    std::cout << "What would you like to do?" << std::endl;
    std::cout << std::endl;
    for (size_t i = 0; i < Num_choices; i++) {
      std::cout << " " << i + 1 << ". ";
      choices[i].printChoice(gadgets_story);
    }
  }
  else if (pageType == Win) {
    std::cout << "Congratulations! You have won. Hooray!" << std::endl;
  }
  else if (pageType == Lose) {
    std::cout << "Sorry, you have lost. Better luck next time!" << std::endl;
  }
}

page::~page() {
}
// stuff for story
story::story() : gadgets_story(), Num_pages(0), pages() {
}
story::story(std::map<std::string, long int> _gadgets_story,
             size_t _Num_pages,
             std::vector<page> _pages) :
    gadgets_story(_gadgets_story), Num_pages(_Num_pages), pages(_pages) {
}
story::story(const story & rhs) :
    gadgets_story(rhs.gadgets_story), Num_pages(rhs.Num_pages), pages(rhs.pages) {
}
story & story::operator=(const story & rhs) {
  if (this != &rhs) {
    gadgets_story = rhs.gadgets_story;
    Num_pages = rhs.Num_pages;
    pages = rhs.pages;
  }
  return *this;
}
void story::printStory(bool just_print_everything) {
  for (size_t i = 0; i < Num_pages; i++) {
    pages[i].printPage(just_print_everything, gadgets_story);
  }
}

void story::playStory() {
  // for Step-2 and 4
  bool print_everything = false;
  size_t current_pg_Num = 0;
  // read a number from user
  while (pages[current_pg_Num].getpageType() == Normal) {
    // gather page info
    // for Step-4: firstly gotta update gadgets_story
    std::map<std::string, long int> gadgets_curnt_pg = pages[current_pg_Num].getGadgePG();
    std::map<std::string, long int>::iterator it_pg;
    std::map<std::string, long int>::iterator it_story;
    for (it_pg = gadgets_curnt_pg.begin(); it_pg != gadgets_curnt_pg.end(); it_pg++) {
      if (gadgets_story.find(it_pg->first) != gadgets_story.end()) {
        it_story = gadgets_story.find(it_pg->first);
        it_story->second = it_pg->second;
      }
    }
    // print the page to the user and gather info about the page
    pages[current_pg_Num].printPage(print_everything, gadgets_story);
    size_t user_input_choice = 0;
    std::string user_input_choice_str;
    size_t current_num_choices = 0;
    current_num_choices = pages[current_pg_Num].getNumChoices();
    std::vector<choice> choices_local = pages[current_pg_Num].getChoices();
    std::pair<std::string, long int> gadget_local;
    do {
      // get user input
      getline(std::cin, user_input_choice_str);
      // see if it meets the requirement of Step-2
      if (!valid_choiceNum_str(user_input_choice_str, current_num_choices)) {
        std::cout << "That is not a valid choice, please try again" << std::endl;
      }
      // see if it meets the requirement of Step-4
      else if (hit_unavilable(
                   user_input_choice_str, gadgets_story, gadget_local, choices_local)) {
        std::cout << "That choice is not available at this time, please try again"
                  << std::endl;
      }
      // if all good we update the page number
      else {
        user_input_choice = strtol(user_input_choice_str.c_str(), NULL, 10);
        current_pg_Num = choices_local[user_input_choice - 1].getnextpgNum();
        break;
      }
    } while (true);
  }
  // when arriving at a page that's either Win or Loose:
  // just print the page and the game would terminate
  pages[current_pg_Num].printPage(print_everything, gadgets_story);
}

void story::printWinPath() const {
  // for Step-3
  // using DFS and Implementation
  // variable initialization:
  size_t current_pg_Num = 0;
  std::set<size_t> visited_pgs;
  std::vector<std::vector<size_t> > pgs_to_take;      //todo
  std::vector<size_t> pgs_have_taken;                 //path
  std::vector<std::vector<size_t> > choices_to_take;  //todo for choices
  std::vector<size_t> choices_have_taken;             //path for choices
  std::vector<std::vector<size_t> > lovely_paths;
  pgs_have_taken.push_back(current_pg_Num);
  std::vector<size_t> all_the_pgNum;
  bool valid_path_exists = false;
  pgs_to_take.push_back(pgs_have_taken);
  choices_to_take.push_back(choices_have_taken);
  while ((pgs_to_take.size() != 0) || (visited_pgs.size() != Num_pages)) {
    // for pages
    pgs_have_taken = pgs_to_take.back();
    pgs_to_take.pop_back();
    // for choices
    choices_have_taken = choices_to_take.back();
    choices_to_take.pop_back();
    // update current page number
    current_pg_Num = pgs_have_taken.back();
    if (pages[current_pg_Num].getpageType() == Win) {
      valid_path_exists = true;
      visited_pgs.insert(current_pg_Num);
      lovely_paths.push_back(pgs_have_taken);
      lovely_paths.push_back(choices_have_taken);
    }
    else if (pages[current_pg_Num].getpageType() == Lose) {
      visited_pgs.insert(current_pg_Num);
    }
    else if (visited_pgs.find(current_pg_Num) == visited_pgs.end()) {
      visited_pgs.insert(current_pg_Num);
      pgs_have_taken.push_back(pages[current_pg_Num].getChoices()[0].getnextpgNum());
      choices_have_taken.push_back(1);
      pgs_to_take.push_back(pgs_have_taken);
      choices_to_take.push_back(choices_have_taken);
      for (size_t i = 1; i < pages[current_pg_Num].getNumChoices(); i++) {
        pgs_have_taken.pop_back();
        choices_have_taken.pop_back();
        pgs_have_taken.push_back(pages[current_pg_Num].getChoices()[i].getnextpgNum());
        choices_have_taken.push_back(i + 1);
        pgs_to_take.push_back(pgs_have_taken);
        choices_to_take.push_back(choices_have_taken);
      }
    }
  }
  // print out the results
  if (valid_path_exists) {
    for (size_t i = 0; i < lovely_paths.size() / 2; i++) {
      size_t lovely_paths_i_size = lovely_paths[2 * i].size();
      for (size_t j = 0; j < lovely_paths_i_size - 1; j++) {
        std::cout << lovely_paths[2 * i][j] << "(" << lovely_paths[2 * i + 1][j] << ")"
                  << ",";
      }
      std::cout << lovely_paths[2 * i][lovely_paths_i_size - 1] << "(win)";
      std::cout << std::endl;
    }
  }
  else {
    std::cout << "This story is unwinnable!" << std::endl;
  }
}

story::~story() {
}
// stuff for mains

// Step-1:
story readAStory(const char * directory) {
  std::string storydirectory;
  storydirectory = std::string(directory) + std::string("/story.txt");
  std::ifstream fileread(storydirectory.c_str());
  if (fileread.fail()) {
    std::cerr << "at readAStory: Input File Cannot be Opened " << storydirectory
              << std::endl;
    exit(EXIT_FAILURE);
  }
  // variable initialization
  std::string oneline;
  std::vector<page> goodpages;
  std::vector<size_t> page_Ive_seen;
  std::vector<size_t> nextpages_in_choices;
  // for Step-4:
  std::map<std::string, long int> gadgets_seen;
  // step2.3c: at least one Win page, one Lose page
  size_t counter_winpg = 0;
  size_t counter_losepg = 0;
  while (!fileread.eof()) {
    std::getline(fileread, oneline);
    // parse it
    if ((oneline.find('@') != std::string::npos) &&
        (oneline.find(':') != std::string::npos) &&
        (oneline.find('@') < oneline.find(':'))) {
      // found at least one @ and whose index is smaller than index of :, assume its a line of page
      page mypage;
      mypage = readAPage(oneline, directory);
      if (!page_Ive_seen.empty()) {
        if (std::binary_search(
                page_Ive_seen.begin(), page_Ive_seen.end(), mypage.getpageNum())) {
          std::cerr << "Have Already Read This Page!" << std::endl;
          exit(EXIT_FAILURE);
        }
        else if (mypage.getpageNum() <= page_Ive_seen.back()) {
          std::cerr << "Page Declarations Must Appear in Order!" << std::endl;
          exit(EXIT_FAILURE);
        }
      }
      if (mypage.getpageType() == Win) {
        counter_winpg += 1;
      }
      else if (mypage.getpageType() == Lose) {
        counter_losepg += 1;
      }
      page_Ive_seen.push_back(mypage.getpageNum());
      goodpages.push_back(mypage);
    }
    else if ((oneline.find('$') != std::string::npos) &&
             (oneline.find('=') != std::string::npos) &&
             (oneline.find('$') < oneline.find('='))) {
      // found at least one $ and whose index is smaller than index of =, assume its a line of gadget of a page
      std::pair<std::string, long int> gadgets_pg_pair;
      size_t curnt_pg_num = 0;
      gadgets_pg_pair = readGadget_pg(oneline, curnt_pg_num);
      if (!std::binary_search(page_Ive_seen.begin(), page_Ive_seen.end(), curnt_pg_num)) {
        std::cerr << "Have Never Seen This Page Before!" << std::endl;
        exit(EXIT_FAILURE);
      }
      for (size_t i = 0; i < goodpages.size(); i++) {
        if (goodpages[i].getpageNum() == curnt_pg_num) {
          goodpages[i].addGadget(gadgets_pg_pair);
          break;
        }
      }
      // Step-4 requirement: If your story encounters a variable which has not been set, it should treat it as having a value of 0.
      gadgets_pg_pair.second = 0;
      gadgets_seen.insert(gadgets_pg_pair);
    }
    else if ((oneline[0] == '\n') || (oneline.empty())) {
      continue;
    }
    else {
      // found no @ nor $ so assume its a line of choice
      choice mychoice;
      mychoice = readAChoice(oneline);
      if (!std::binary_search(
              page_Ive_seen.begin(), page_Ive_seen.end(), mychoice.getcurrentpgNum())) {
        std::cerr << "Have Never Seen This Page Before!" << std::endl;
        exit(EXIT_FAILURE);
      }
      nextpages_in_choices.push_back(mychoice.getnextpgNum());
      // add the current choice into corrosponding pages
      for (size_t i = 0; i < goodpages.size(); i++) {
        if (goodpages[i].getpageNum() == mychoice.getcurrentpgNum()) {
          goodpages[i].addChoice(mychoice);
          break;
        }
      }
    }
  }
  // perform some checking before constructing a story object
  // step2.3a: nextpgnum shown in the choices should be a valid page number
  std::vector<size_t>::iterator it;
  for (it = nextpages_in_choices.begin(); it != nextpages_in_choices.end(); it++) {
    if (!std::binary_search(page_Ive_seen.begin(), page_Ive_seen.end(), *it)) {
      std::cerr << "Some Pages in the Choices have never shown up!" << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  // step2.3b: pages except page 0 should somewhat being referenced
  std::sort(nextpages_in_choices.begin(), nextpages_in_choices.end());
  for (it = page_Ive_seen.begin() + 1; it != page_Ive_seen.end(); it++) {
    if (!std::binary_search(
            nextpages_in_choices.begin(), nextpages_in_choices.end(), *it)) {
      std::cerr << "Some Pages are not being referenced by any choice!" << *it
                << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  // step2.3c:
  if ((counter_losepg == 0) || (counter_winpg == 0)) {
    std::cerr << "At least one lose/win page!" << std::endl;
    exit(EXIT_FAILURE);
  }
  story mystory(gadgets_seen, goodpages.size(), goodpages);
  return mystory;
}

page readAPage(std::string oneline, const char * directory) {
  size_t pgNUM = 0;
  pageTypes pgType = Normal;
  std::string pgFile;
  size_t numpgs = 0;
  std::vector<choice> pgchoices;
  // now parse oneline to fill in such variables
  // gotta parse on ':'
  // first half, number@type
  std::string num_n_type = oneline.substr(0, oneline.find(":"));
  int aadr_count = 0;
  size_t aadr_index = 0;
  for (size_t i = 0; i < num_n_type.length(); i++) {
    if (num_n_type[i] == '@') {
      aadr_count += 1;
      aadr_index = i;
    }
  }
  if (aadr_count != 1) {
    std::cerr << "Should have exactly one '@' to seperate pgnum and pgtypes!"
              << std::endl;
    exit(EXIT_FAILURE);
  }
  // 1st half of first half, number
  std::string num_str = num_n_type.substr(0, aadr_index);
  pgNUM = numstr(num_str);
  // 2nd half of first half, type
  std::string type_str = num_n_type.substr(aadr_index + 1);
  if (type_str == "N") {
    pgType = Normal;
  }
  else if (type_str == "W") {
    pgType = Win;
  }
  else if (type_str == "L") {
    pgType = Lose;
  }
  else {
    std::cerr << "Input page type is neither normal, win nor lose" << std::endl;
    exit(EXIT_FAILURE);
  }
  // second half, filename
  pgFile = oneline.substr(oneline.find(":") + 1);
  assert(pgFile.length() != 0);
  pgFile = std::string(directory) + std::string("/") + pgFile;
  // for Step-4: construct an empty map
  std::map<std::string, long int> pg_ggts;
  // construct aPage and return it
  page aPage(pgNUM, pg_ggts, pgType, pgFile, numpgs, pgchoices);
  return aPage;
}

choice readAChoice(std::string oneline) {
  size_t curntpgNum = 0;
  size_t nxtpgNum = 0;
  std::string txtMsg;
  // make sure at least 2 ':'
  int colon_count = 0;
  std::vector<int> colon_indices;
  for (size_t i = 0; i < oneline.length(); i++) {
    if (oneline[i] == ':') {
      colon_count += 1;
      colon_indices.push_back(i);
      if (colon_count == 2) {
        break;
      }
    }
  }
  if (colon_count <= 1) {
    std::cerr << "Choice line should have at least two colons!" << std::endl;
    exit(EXIT_FAILURE);
  }
  else if ((colon_indices[1] - colon_indices[0]) == 1) {
    std::cerr << "First two colons should not be in consecutive order!" << std::endl;
    exit(EXIT_FAILURE);
  }
  // now parse on ':'
  std::string curntnum_ggt_str = oneline.substr(0, colon_indices[0]);
  std::string curntnum_str = curntnum_ggt_str.substr(0, curntnum_ggt_str.find('['));
  // now parse on '=', for step-4
  std::string ggt_name_str;
  std::string ggt_value_str;
  long int ggt_value;
  bool true_gadget = false;
  if (curntnum_ggt_str.length() != curntnum_str.length()) {
    true_gadget = true;
  }
  if (true_gadget) {
    assert((curntnum_ggt_str.find('[') != std::string::npos) &&
           (curntnum_ggt_str.find('=') != std::string::npos) &&
           (curntnum_ggt_str.find(']') != std::string::npos) &&
           (curntnum_ggt_str.find('[') < curntnum_ggt_str.find('=')) &&
           (curntnum_ggt_str.find('=') < curntnum_ggt_str.find(']')));
    ggt_name_str = curntnum_ggt_str.substr(
        curntnum_ggt_str.find('[') + 1,
        curntnum_ggt_str.find('=') - curntnum_ggt_str.find('[') - 1);
    ggt_value_str = curntnum_ggt_str.substr(curntnum_ggt_str.find('=') + 1,
                                            curntnum_ggt_str.find(']'));
    ggt_value = strtol(ggt_value_str.c_str(), NULL, 10);
  }
  std::string nxtnum_str =
      oneline.substr(colon_indices[0] + 1, colon_indices[1] - colon_indices[0] - 1);
  txtMsg = oneline.substr(colon_indices[1] + 1);
  curntpgNum = numstr(curntnum_str);
  nxtpgNum = numstr(nxtnum_str);
  // for Step-4
  std::pair<std::string, long int> gadgett;
  std::pair<std::string, long int> gadget(ggt_name_str, ggt_value);
  // construct a choice object and return it
  choice aChoice(curntpgNum, gadget, nxtpgNum, txtMsg);
  choice bChoice(curntpgNum, gadgett, nxtpgNum, txtMsg);
  if (true_gadget) {
    return aChoice;
  }
  else {
    return bChoice;
  }
}

size_t numstr(std::string num_str) {
  // convert a string into numbers
  assert(num_str.length() != 0);
  std::string::iterator it;
  bool digit_in_front = false;
  for (it = num_str.begin(); it != num_str.end(); it++) {
    if ((!isdigit(*it)) && (*it != ' ')) {
      std::cerr << "Contains invalid char!" << std::endl;
      exit(EXIT_FAILURE);
    }
    if (digit_in_front && (*it == ' ')) {
      std::cerr << "Digits should not followed by white speces!" << std::endl;
      exit(EXIT_FAILURE);
    }
    else if (isdigit(*it)) {
      digit_in_front = true;
    }
  }
  return strtol(num_str.c_str(), NULL, 10);
}

void printTxt(std::string filedirectory) {
  std::ifstream fileread(filedirectory.c_str());
  if (fileread.fail()) {
    std::cerr << "Input File Cannot be Opened " << filedirectory << std::endl;
    exit(EXIT_FAILURE);
  }
  std::string oneline;
  while (!fileread.eof()) {
    std::getline(fileread, oneline);
    std::cout << oneline << std::endl;
  }
}

bool valid_choiceNum_str(std::string & user_input_choice_str,
                         size_t & current_num_choices) {
  // used for Step-2 user-input checking
  if (user_input_choice_str.length() == 0) {
    return false;
  }
  // check through the user-input string contains only whitespaces and (followed by) digits
  std::string::iterator it;
  bool digit_in_front = false;
  for (it = user_input_choice_str.begin(); it != user_input_choice_str.end(); it++) {
    if ((!isdigit(*it)) && (*it != ' ')) {
      return false;
    }
    if (digit_in_front && (*it == ' ')) {
      return false;
    }
    else if (isdigit(*it)) {
      digit_in_front = true;
    }
  }
  size_t number = 0;
  number = strtol(user_input_choice_str.c_str(), NULL, 10);
  if ((number > current_num_choices) || (number < 1)) {
    return false;
  }
  return true;
}

// Step-3: DFS

// Step-4: Gadgets
std::pair<std::string, long int> readGadget_pg(std::string oneline,
                                               size_t & curnt_pg_num) {
  std::string gadget_name_str;
  long int gadget_value = 0;
  // now parse oneline to fill in such variables
  // gotta parse on '='
  // first half, pagenum$gadget_name_str
  std::string num_n_ggt = oneline.substr(0, oneline.find("="));
  int dlr_count = 0;
  size_t dlr_index = 0;
  for (size_t i = 0; i < num_n_ggt.length(); i++) {
    if (num_n_ggt[i] == '$') {
      dlr_count += 1;
      dlr_index = i;
    }
  }
  if (dlr_count != 1) {
    std::cerr << "Should have exactly one '@' to seperate pgnum and pgtypes!"
              << std::endl;
    exit(EXIT_FAILURE);
  }
  // 1st half of first half, pagenum
  std::string num_str = num_n_ggt.substr(0, dlr_index);
  curnt_pg_num = numstr(num_str);
  // 2nd half of first half, gadget_name_str
  gadget_name_str = num_n_ggt.substr(dlr_index + 1);
  // second half, value
  std::string gadget_value_str;
  gadget_value_str = oneline.substr(oneline.find("=") + 1);
  assert(gadget_value_str.length() != 0);
  std::string::iterator it;
  bool digit_in_front = false;
  for (it = gadget_value_str.begin(); it != gadget_value_str.end(); it++) {
    if ((!isdigit(*it)) && (*it != ' ')) {
      std::cerr << "Contains invalid char!" << std::endl;
      exit(EXIT_FAILURE);
    }
    if (digit_in_front && (*it == ' ')) {
      std::cerr << "Digits should not followed by white speces!" << std::endl;
      exit(EXIT_FAILURE);
    }
    else if (isdigit(*it)) {
      digit_in_front = true;
    }
  }
  gadget_value = strtol(gadget_value_str.c_str(), NULL, 10);
  // construct aPage and return it
  std::pair<std::string, long int> gadgets_pg_pair(gadget_name_str, gadget_value);
  return gadgets_pg_pair;
}

bool is_ggt_match(std::map<std::string, long int> & gadgets_story,
                  std::pair<std::string, long int> & gadget) {
  // check if the gadget is inside the story and compair their values
  std::map<std::string, long int>::iterator it = gadgets_story.find(gadget.first);
  if (it != gadgets_story.end()) {
    if (it->second == gadget.second) {
      return true;
    }
  }
  return false;
}

bool hit_unavilable(std::string & user_input_choice_str,
                    std::map<std::string, long int> & gadgets_story,
                    std::pair<std::string, long int> & gadget_local,
                    std::vector<choice> & choices_local) {
  // it is guaranteed that user_input_choice is valid for the standard in Step-2
  size_t user_input_choice = strtol(user_input_choice_str.c_str(), NULL, 10);
  gadget_local = choices_local[user_input_choice - 1].getGadget();
  if ((gadgets_story.empty() == 1) ||
      (gadgets_story.find(gadget_local.first) == gadgets_story.end())) {
    return false;
  }
  else {
    return !is_ggt_match(gadgets_story, gadget_local);
  }
}
