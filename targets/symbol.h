#ifndef __OG_TARGETS_SYMBOL_H__
#define __OG_TARGETS_SYMBOL_H__

#include <string>
#include <memory>
#include <cdk/types/basic_type.h>

namespace og {

  class symbol {
    std::shared_ptr<cdk::basic_type> _type;
    int _offset = 0;
    std::string _name;
    int _function; // 0 -> Not a function; 1 -> Function; 2 -> Procedure
    std::vector<std::shared_ptr<cdk::basic_type>> *_arguments;
    int _declared; // 1 -> declared; 2 -> defined;
    std::shared_ptr<cdk::basic_type> _return_type;

  public:
    symbol(std::shared_ptr<cdk::basic_type> type, const std::string &name, int function, 
    std::vector<std::shared_ptr<cdk::basic_type>> *arguments, int declared, std::shared_ptr<cdk::basic_type> return_type) :
        _type(type), _name(name), _function(function), _arguments(arguments), _declared(declared), _return_type(return_type) {
    }

    virtual ~symbol() {
      // EMPTY
    }

    std::shared_ptr<cdk::basic_type> type() const {
      return _type;
    }
    void type(std::shared_ptr<cdk::basic_type> type){
      _type = type;
    }
    bool is_typed(cdk::typename_type name) const {
      return _type->name() == name;
    }
    const std::string &name() const {
      return _name;
    }
    bool global() const {
      return _offset == 0;
    }
    int offset() const {
      return _offset;
    }
    void set_offset(int offset) {
      _offset = offset;
    }
    int function() {
      return _function;
    }
    void function(int value) {
      _function = value;
    }
    std::vector<std::shared_ptr<cdk::basic_type>> *arguments(){
      return _arguments;
    }
    void arguments(std::vector<std::shared_ptr<cdk::basic_type>> *arguments){
      _arguments = arguments;
    }
    int declared() {
      return _declared;
    }
    void declared(int declared){
      _declared = declared;
    }
    std::shared_ptr<cdk::basic_type> return_type(){
      return _return_type;
    }
    void return_type(std::shared_ptr<cdk::basic_type> return_type){
      _return_type = return_type;
    }
    
  };

  inline auto make_symbol(std::shared_ptr<cdk::basic_type> type, const std::string &name, int function, 
  std::vector<std::shared_ptr<cdk::basic_type>> *arguments, int declared, std::shared_ptr<cdk::basic_type> return_type){
    return std::make_shared<symbol>(type, name, function, arguments, declared, return_type);
  }

} // og

#endif
