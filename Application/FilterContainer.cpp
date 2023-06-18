#include <FilterContainer.h>

#include <Application.h>

namespace UDA {
    Driver::DFSDMFilter& getFilterByIndex(std::size_t index) {
        return Application::getInstante().getFilterByIndex(index);
    }
}
