#include <expected>
#include <string>
#include <unordered_set>
namespace Stellatus{
struct State{
private:
    enum class _State{};  
    std::unordered_set<_State, std::string> _mapping_table; 
};
template <typename _Ty, typename _Error_Msg>
struct Result{
    // std::expected<_Ty, State> _ret;
};

}