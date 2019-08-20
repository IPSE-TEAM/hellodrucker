#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/name.hpp>

using namespace eosio;

CONTRACT hellodrucker : public contract {
  public:
    using contract::contract;
    hellodrucker(eosio::name receiver, eosio::name code, datastream<const char*> ds):contract(receiver, code, ds) {}

    ACTION typein(name address,std::string sponsorname,uint32_t period,asset incentive);
    ACTION launch(name address);
    ACTION settle(name address);
    ACTION slash(name address,name member);
    ACTION cancelmember(name address,name member);
    ACTION cancelsponso(name address);
    ACTION withdrawbal(name address,asset balance);
    ACTION accept(name address,name member);
    ACTION join(name address,name sponsoraddr,std::string membername);
    ACTION leave(name address,name sponsoraddr);
    ACTION send(name address,name to,asset amount);
    ACTION withdraw(name address);
    ACTION delmember(name teammember);
    
    using typein_action = action_wrapper<"typein"_n, &hellodrucker::typein>;
    using launch_action = action_wrapper<"launch"_n, &hellodrucker::launch>;
    using settle_action = action_wrapper<"settle"_n, &hellodrucker::settle>;
    using slash_action = action_wrapper<"slash"_n, &hellodrucker::slash>;
    using cancelmember_action = action_wrapper<"cancelmember"_n, &hellodrucker::cancelmember>;
    using cancelsponso_action = action_wrapper<"cancelsponso"_n, &hellodrucker::cancelsponso>;
    using withdrawbal_action = action_wrapper<"withdrawbal"_n, &hellodrucker::withdrawbal>;
    using accept_action = action_wrapper<"accept"_n, &hellodrucker::accept>;
    using join_action = action_wrapper<"join"_n, &hellodrucker::join>;
    using leave_action = action_wrapper<"leave"_n, &hellodrucker::leave>;
    using send_action = action_wrapper<"send"_n, &hellodrucker::send>;
    using withdraw_action = action_wrapper<"withdraw"_n, &hellodrucker::withdraw>;
    using delmember_action = action_wrapper<"delmember"_n, &hellodrucker::delmember>;

  private:
    TABLE sponsor {
      name address;
      std::string sponsorname;
      asset totaltoken;
      asset balance;
      uint32_t  members;
      asset incentive;
      asset slashtoken;
      uint64_t timestamp;
      uint64_t period;
      uint32_t phase;
      std::string status;
      uint64_t primary_key() const { return address.value;}
    };
    typedef eosio::multi_index<"sponsor"_n, sponsor> sponsor_index;
    
    TABLE teammember {
      name address;
      name sponsoraddr;
      std::string membername;
      asset in;
      asset out;
      asset outbalance;
      asset incentive;
      asset balance;
      uint32_t phase;
      std::string status;
      uint64_t primary_key() const { return address.value;}
    };
    typedef eosio::multi_index<"teammember"_n, teammember> teammember_index;
};

EOSIO_DISPATCH(hellodrucker, (typein)(launch)(settle)(slash)(cancelmember)(cancelsponso)(withdrawbal)(accept)(join)(leave)(send)(withdraw)(delmember))
