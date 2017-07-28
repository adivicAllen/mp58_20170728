#ifndef _ADIVIC_GLOABL_HPP_
#define _ADIVIC_GLOABL_HPP_
#include <QObject>
#include <QDebug>
#include <QThread>

namespace internal {

//! Base class for types that should not be copied or assigned.
class no_copy  : public QObject
{
Q_OBJECT
private:    
    no_copy( const no_copy& other);
    no_copy& operator=(const no_copy&);
public:
    no_copy() {};
};

}

#endif
