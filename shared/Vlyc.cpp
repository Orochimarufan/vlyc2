#include "Vlyc.h"

namespace Vlyc {

QString version()
{
    return QStringLiteral(VLYC_VERSION);
}

/*QString changeset()
{
    return QStringLiteral(VLYC_REVISION);
}*/

quint64 hexversion()
{
    return VLYC_HEXVERSION;
}

}
