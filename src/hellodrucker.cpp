#include <hellodrucker.hpp>
#include <eosio/system.hpp> 
#include <eosio/eosio.hpp>
#include <eosio/contract.hpp>
#include <eosio/time.hpp>

[[eosio::action]] 
void hellodrucker::typein(name sponsoraddr,std::string sponsorname,uint32_t period,asset incentive) {
  require_auth(sponsoraddr);
  check(period>86400,"period must over 1 day");
  sponsor_index sponsortable("hellodrucker"_n,sponsoraddr.value);
  auto itr = sponsortable.find(sponsoraddr.value);
  if(itr != sponsortable.end()){
    print("registered");
  }else{
    symbol symbol_ = symbol("POST", 4);
    asset asset_ = asset(0.0000,symbol_);
    sponsortable.emplace("hellodrucker"_n, [&]( auto& row){
      row.address = sponsoraddr;
      row.sponsorname = sponsorname;
      row.totaltoken = asset_;
      row.balance = asset_;
      row.members = 0;
      row.incentive = incentive;
      row.slashtoken = asset_;
      row.timestamp = current_time_point().sec_since_epoch();
      row.period = period;
      row.phase = 0;
      row.status = "registered";
    });
    print("register success,let the team members join you now.");
  }
};

[[eosio::action]] 
void hellodrucker::launch(name address){
  require_auth(address);
  sponsor_index sponsortable("hellodrucker"_n,address.value);
  auto itr = sponsortable.find(address.value);
  if(itr != sponsortable.end()){
    sponsortable.modify(itr,"hellodrucker"_n,[&]( auto& row ){
      row.timestamp = current_time_point().sec_since_epoch();
      row.phase = row.phase + 1;
      row.status = "starting";
    });
    print("you launch the incentive plan success.");
  }else{
    print("you are not a sponsor now!");
  }
};

[[eosio::action]] 
void hellodrucker::settle(name address){
  require_auth(address);
  sponsor_index sponsortable("hellodrucker"_n,address.value);
  auto itr = sponsortable.find(address.value);
  if(itr != sponsortable.end()){
    std::string status = itr -> status;
    if(status == "starting"){
      uint64_t timestamp = itr -> timestamp;
      uint64_t period = itr -> period;
      uint64_t timediff = current_time_point().sec_since_epoch() - timestamp;
      uint64_t period_ = period * 0.8;
      if(timediff>period_){
        sponsortable.modify(itr,"hellodrucker"_n,[&]( auto& row ){
          row.status = "settling";
        });
        print("you settling success.");
      }else{
        print("over 80% \\period time,you could settle.");
      }
    }else{
      print("the status of sponsor did not support settle.");
    }
  }else{
    print("you are not a sponsor now!");
  }
};

[[eosio::action]] 
void hellodrucker::slash(name address,name member){
  require_auth(address);
  teammember_index teammembertable("hellodrucker"_n,member.value);
  auto team_itr = teammembertable.find(member.value);
  if(team_itr != teammembertable.end()){
    name sponsoraddr = team_itr -> sponsoraddr;
    if(sponsoraddr.value == address.value){
      sponsor_index sponsortable("hellodrucker"_n,address.value);
      auto sponsor_itr = sponsortable.find(address.value);
      std::string status = sponsor_itr -> status;
      if(status == "settling"){
        asset outbalance = team_itr -> outbalance;
        symbol symbol_ = symbol("POST", 4);
        asset zero = asset(0.0000,symbol_);
        teammembertable.modify(team_itr,"hellodrucker"_n,[&]( auto& row ){
          row.outbalance = zero;
        });
        asset sponsor_balance = sponsor_itr -> balance;
        asset slashtoken = sponsor_itr -> slashtoken;
        asset balance_ = asset(sponsor_balance.amount + outbalance.amount,symbol_);
        asset slashtoken_ = asset(slashtoken.amount + outbalance.amount,symbol_);
        sponsortable.modify(sponsor_itr,"hellodrucker"_n,[&]( auto& row ){
          row.balance = balance_;
          row.slashtoken = slashtoken_;
        });
        print("you slash success.");
      }else{
        print("the sponsor status did not support slash.");
      }
    }else{
      print("the member is not your team member.");
    }
  }else{
    print("the member did not exists.");
  }
};

[[eosio::action]] 
void hellodrucker::cancelmember(name address,name member){
  require_auth(address);
  teammember_index teammembertable("hellodrucker"_n,member.value);
  auto itr = teammembertable.find(member.value);
  if(itr != teammembertable.end()){
    name sponsoraddr = itr -> sponsoraddr;
    if(address.value == sponsoraddr.value){
      teammembertable.modify(itr,"hellodrucker"_n,[&]( auto& row ){
        row.status = "cancelled";
      });
      sponsor_index sponsortable("hellodrucker"_n,address.value);
      auto sponsor_itr = sponsortable.find(address.value);
      if(sponsor_itr != sponsortable.end()){
        sponsortable.modify(sponsor_itr,"hellodrucker"_n,[&]( auto& row ){
          row.members = row.members - 1;
        });
      }
      print("you cancel the team member success.");
    }else{
      print("the member did not in you team.");
    }
  }else{
    print("the member did not exists.");
  }
};

[[eosio::action]] 
void hellodrucker::cancelsponso(name address){
  require_auth(address);
  sponsor_index sponsortable("hellodrucker"_n,address.value);
  auto itr = sponsortable.find(address.value);
  if(itr != sponsortable.end()){
    asset balance = itr -> balance;
    if(balance.amount>0){
      print("please withdraw the balance.");
    }else{
      sponsortable.erase(itr);
      print("you cancel sponsor success.");
    }
  }else{
    print("sponsor did not exists!");
  }
};

[[eosio::action]] 
void hellodrucker::withdrawbal(name address,asset amount){
  require_auth(address);
  sponsor_index sponsortable("hellodrucker"_n,address.value);
  auto itr = sponsortable.find(address.value);
  if(itr != sponsortable.end()){
    asset balance = itr -> balance;
    if(amount.amount <= balance.amount){
      uint32_t members = itr -> members;
      asset incentive = itr -> incentive;
      symbol symbol_ = symbol("POST", 4);
      auto b = balance.amount - members*incentive.amount;
      if(b >= amount.amount){
        asset balance_ = asset(balance.amount - amount.amount,symbol_);
        sponsortable.modify(itr,"hellodrucker"_n,[&]( auto& row ){
          row.balance = balance_;
        });
        action(
          permission_level{_self,"active"_n} ,
          "ipsecontract"_n,"transfer"_n,
          std::make_tuple(_self,address,amount,"sponsor withdraw the token.")
        ).send();
        print("withdraw success.");
      }
    }else{
      print("you withdraw over the balance.");
    }
  }else{
    print("sponsor did not exists!");
  }
};

[[eosio::action]] 
void hellodrucker::accept(name sponsoraddr,name member){
  require_auth(sponsoraddr);
  sponsor_index sponsortable("hellodrucker"_n,sponsoraddr.value);
  auto itr = sponsortable.find(sponsoraddr.value);
  if(itr != sponsortable.end()){
    teammember_index teammembertable("hellodrucker"_n,member.value);
    auto team_itr = teammembertable.find(member.value);
    if(team_itr != teammembertable.end()){
      asset balance = itr -> balance;
      asset incentive = itr -> incentive;
      if(balance.amount >= incentive.amount){
        std::string sponsor_status = itr -> status;
        if(sponsor_status == "settling"){
          teammembertable.modify(team_itr,"hellodrucker"_n,[&]( auto& row ){
            row.incentive = incentive;
            row.balance = incentive;
            row.phase = row.phase + 1;
            row.status = "joined";
          });
        }else{
          teammembertable.modify(team_itr,"hellodrucker"_n,[&]( auto& row ){
            row.incentive = incentive;
            row.balance = incentive;
            row.status = "joined";
          });
          
        }
        symbol symbol_ = symbol("POST", 4);
        asset balance_ = asset(balance.amount - incentive.amount,symbol_);
        sponsortable.modify(itr,"hellodrucker"_n,[&]( auto& row ){
          row.balance = balance_;
          row.members = row.members + 1;
        });
        print("you accept team member success.");
      }else{
        print("you have not enough token!");
      }
    }else{
      print("the team member is not exists!");
    }
  }else{
    print("you are not a sponsor now!");
  }
};


[[eosio::action]] 
void hellodrucker::join(name address,name sponsoraddr,std::string membername){
  require_auth(address);
  sponsor_index sponsortable("hellodrucker"_n,sponsoraddr.value);
  auto itr = sponsortable.find(sponsoraddr.value);
  if(itr != sponsortable.end()){
    teammember_index teammembertable("hellodrucker"_n,address.value);
    auto teammember_itr = teammembertable.find(address.value);
    if(teammember_itr != teammembertable.end()){
      print("you could join only one team!");
    }else{
      symbol symbol_ = symbol("POST", 4);
      asset asset_ = asset(0.0000,symbol_);
      uint32_t phase = itr -> phase;
      teammembertable.emplace("hellodrucker"_n, [&]( auto& row){
        row.address = address;
        row.sponsoraddr = sponsoraddr;
        row.membername = membername;
        row.in = asset_;
        row.out = asset_;
        row.outbalance = asset_;
        row.incentive = asset_;
        row.balance = asset_;
        row.phase = phase;
        row.status = "joining";
      });
      print("you joining now,wait sponsor accept you!");
    }
  }else{
    print("sponsor did not exists!");
  }
  
};

[[eosio::action]] 
void hellodrucker::leave(name address,name sponsoraddr){
  require_auth(address);
  sponsor_index sponsortable("hellodrucker"_n,sponsoraddr.value);
  auto itr = sponsortable.find(sponsoraddr.value);
  if(itr != sponsortable.end()){
    teammember_index teammembertable("hellodrucker"_n,address.value);
    auto teammember_itr = teammembertable.find(address.value);
    if(teammember_itr != teammembertable.end()){
      std::string member_status = teammember_itr -> status;
      if(member_status == "joined"){
        std::string sponsor_status = itr -> status;
        if(sponsor_status == "settling"){
          asset balance = teammember_itr -> balance;
          if(balance.amount > 0){
            action(
              permission_level{_self,"active"_n} ,
              "ipsecontract"_n,"transfer"_n,
              std::make_tuple(_self,address,balance,"leave and share the token.")
            ).send();
          }
        }
      }
      
      sponsortable.modify(itr,"hellodrucker"_n,[&]( auto& row ){
        row.members = row.members - 1;
      });
      teammembertable.erase(teammember_itr);
      print("you leave success.");
    }else{
      print("you did not join any one team.");    
    }
  }else{
    print("sponsor did not exists!");
  }
};


[[eosio::action]] 
void hellodrucker::send(name address,name to,asset amount){
  require_auth(address);
  check(amount.amount>0,"send token over 0");
  teammember_index teammembertablesend("hellodrucker"_n,address.value);
  teammember_index teammembertablereceiver("hellodrucker"_n,to.value);
  auto teammember_send_itr = teammembertablesend.find(address.value);
  auto teammember_receiver_itr = teammembertablereceiver.find(to.value);
  if(teammember_send_itr != teammembertablesend.end() && teammember_receiver_itr != teammembertablereceiver.end()){
    // check the phase  TODO
    name send_sponsor = teammember_send_itr -> sponsoraddr;
    name receiver_sponsor = teammember_receiver_itr -> sponsoraddr;
    if(send_sponsor.value == receiver_sponsor.value){
      sponsor_index sponsortable("hellodrucker"_n,send_sponsor.value);
      auto sponsor_itr = sponsortable.find(send_sponsor.value);
      std::string sponsor_status = sponsor_itr -> status;
      if(sponsor_status == "starting"){
        asset send_outbalance = teammember_send_itr -> outbalance;
        if(send_outbalance.amount >= amount.amount){
          symbol symbol_ = symbol("POST", 4);
          asset send_out = teammember_send_itr -> out;
          asset outbalance_ = asset(send_outbalance.amount - amount.amount,symbol_);
          asset send_out_ = asset(send_out.amount + amount.amount,symbol_);
          teammembertablesend.modify(teammember_send_itr,"hellodrucker"_n,[&]( auto& row ){
            row.out = send_out_;
            row.outbalance = outbalance_;
          });
          asset receiver_in = teammember_receiver_itr -> in;
          asset receiver_in_ = asset(receiver_in.amount + amount.amount,symbol_);
          teammembertablereceiver.modify(teammember_receiver_itr,"hellodrucker"_n,[&]( auto& row ){
            row.in = receiver_in_;
          });
          
          print("you send token success.");
        }else{
          print("you send token over the out balance");
        }
      }else{
        print("the sponsor status is not starting."); 
      }
    }else{
      print("you and the receiver did not in one team.");
    }
  }else{
    print("you or receiver are not a team member.");
  }
};

[[eosio::action]] 
void hellodrucker::withdraw(name address){
  require_auth(address);
  teammember_index teammembertable("hellodrucker"_n,address.value);
  auto teammember_itr = teammembertable.find(address.value);
  if(teammember_itr != teammembertable.end()){
    name sponsoraddr = teammember_itr -> sponsoraddr;
    sponsor_index sponsortable("hellodrucker"_n,sponsoraddr.value);
    auto sponsor_itr = sponsortable.find(sponsoraddr.value);
    if(sponsor_itr != sponsortable.end()){
      std::string status = sponsor_itr -> status;
      if(status == "settling"){
        symbol symbol_ = symbol("POST", 4);
        asset zero = asset(0.0000,symbol_);
        asset in = teammember_itr -> in;
        asset balance = teammember_itr -> balance;
        asset withdrawtoken = asset(in.amount + balance.amount,symbol_);
        asset sponsor_balance = sponsor_itr -> balance;
        if(sponsor_balance.amount > withdrawtoken.amount){
          teammembertable.modify(teammember_itr,"hellodrucker"_n,[&]( auto& row ){
            row.in = zero;
            row.balance = zero;
            row.out = zero;
            row.outbalance = zero;
            row.phase = row.phase + 1;
          });
          asset sponsor_balance_ = asset(sponsor_balance.amount - withdrawtoken.amount,symbol_);
          sponsortable.modify(sponsor_itr,"hellodrucker"_n,[&]( auto& row ){
            row.balance = sponsor_balance_;
          });
          action(
            permission_level{_self,"active"_n} ,
            "ipsecontract"_n,"transfer"_n,
            std::make_tuple(_self,address,withdrawtoken,"withdraw the token.")
          ).send();
          print("withdraw success.");
        }else{
          print("the sponsor's balance is not enough support withdraw.");
        }
      }else{
        print("the status of sponsor did not support withdraw.");
      }
    }else{
      print("can't find the sponsor.");
    }
  }else{
    print("can't find the team member.");
  }
};

[[eosio::action]] 
void hellodrucker::delmember(name teammember){
  require_auth(_self);
  teammember_index teammembertable("hellodrucker"_n,teammember.value);
  auto teammember_itr = teammembertable.find(teammember.value);
  if(teammember_itr != teammembertable.end()){
    teammembertable.erase(teammember_itr);
  }else{
    print("can't find the team member."); 
  }
};


