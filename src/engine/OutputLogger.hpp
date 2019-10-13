#ifndef _OUTPUT_LOGGER_HPP_
#define _OUTPUT_LOGGER_HPP_
namespace SymbolicRegression
{
class OutputLogger
{
public:
    static OutputLogger *Instance; // Singleton
    OutputLogger();
}
} // namespace SymbolicRegression
#endif