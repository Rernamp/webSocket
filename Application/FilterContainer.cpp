#include <FilterContainer.h>

#include <Application.h>

namespace UDA {
    Driver::DFSDMFilter& getFilter1() {
        return Application::getInstante().getFilter();
    }
    Driver::DFSDMFilter& getFilter2() {
        return Application::getInstante().getFilter();
    }
    Driver::DFSDMFilter& getFilter3() {
        return Application::getInstante().getFilter();
    }
    Driver::DFSDMFilter& getFilter4() {
        return Application::getInstante().getFilter();
    }
}