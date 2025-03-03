#include "Channel.hpp"
#include "algorithm"
#include "../utility/utility.hpp"

Channel::Channel(std::string const & channelName, Postman *postman):
                _postman(postman),
                _channelName(channelName),
                _modes(none) {}

void Channel::addUser(User *user) {
    _userList.push_back(user);
    if (_operList.empty())
        addOper(user);
}

void Channel::addOper(User *user) {
    if (!is_operator(user))
        _operList.push_back(user);
}

const std::string &Channel::get_topic() const               { return _topic; }
int Channel::get_limit() const                              { return _limit; }
const std::string &Channel::get_channelname() const         { return _channelName; }
const std::vector<User *> &Channel::get_userlist() const    { return _userList; }
const std::vector<User *> &Channel::get_operList() const    { return _operList; }

int Channel::get_count_of_users() { return (int)_userList.size(); }


User * Channel::get_user_by_nickname(std::string nickname){
    std::vector<User *> ::iterator start = _userList.begin();
    std::vector<User *> ::iterator end = _userList.end();
    nickname = ut::to_lower(nickname);
    while (start != end) {
        if (nickname == (*start)->get_nickname())
            return *start;
        start++;
    }
    return nullptr;
}


void Channel::removeOper(User *user) {
    std::vector<User *>::iterator start = _operList.begin();
    while (start != _operList.end()) {
        if ((*start) == user) {
            _operList.erase(start);
            break;
        }
        start++;
    }
    if (_operList.empty() && !_userList.empty())
        addOper(_userList[0]);
}

void Channel::removeUserFromChannel(User *user){
    std::vector<User *>::iterator start = _userList.begin();
    while (start != _userList.end()) {
        if ((*start) == user) {
            _userList.erase(start);
            break;
        }
        start++;
    }
    removeOper(user);
}
void Channel::sendAll(const std::string& msg, User *skippedUser) {
    for (std::vector<User *>::iterator start = _userList.begin(); start != _userList.end(); start++){
        if (*start != skippedUser)
            _postman->sendReply((*start)->get_socket(), msg);
    }
}

void Channel::set_topic(const std::string &topic) {
    this->_topic = topic;
}

void Channel::set_limit(int limit) {
    this->_limit = limit;
}

bool Channel::is_in_channel(User *user) const {
    for (std::vector<User *>::const_iterator start = _userList.begin(); start != _userList.end(); start++){
        if (user == (*start))
            return true;
    }
    return false;
}

bool Channel::is_operator(User *user) const {
    for (std::vector<User *>::const_iterator start = _operList.begin(); start != _operList.end(); start++) {
        if (user == (*start))
            return true;
    }
    return false;
}

// bool Channel::is_invited(User * user){
//     std::vector<User *>::iterator start = _inviteList.begin();
//     while (start != _inviteList.end()){
//         if ((*start)->get_socket() == user->get_socket())
//             return true;
//         start++;
//     }
//     return false;
// }

void Channel::set_mode(Mode flag) {
    _modes |= flag;
}

void Channel::unset_mode(Mode flag) {
    _modes &= (~flag);
}

bool Channel::has_mode(Mode flag) const {
    return ((_modes & flag) == flag);
}

// {[+|-]|o|p|s|i|t|n|b|v}
std::string Channel::show_mode() const {
    std::string show;
    if (has_mode(oper))
        show += 'o';
    if (has_mode(invite_only))
        show += 'i';
    if (has_mode(protectedTopic))
        show += 't';
    if (has_mode(limited))
        show += "l " + std::to_string(_limit);

    return show.empty() ? "" : '+' + show;
}
