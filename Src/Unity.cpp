#include "Error.h"
#include "Unity.h"

#include <QObject>


namespace Vreo
{

void Init()
{
    qRegisterMetaType<Vreo::Error>("Vreo::Error");
}

}

