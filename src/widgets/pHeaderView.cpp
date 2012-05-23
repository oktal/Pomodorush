#include "pHeaderView.h"

pHeaderView::pHeaderView( Qt::Orientation orientation, QWidget* parent )
	: QHeaderView( orientation, parent )
{
}

void pHeaderView::resizeEvent( QResizeEvent* event )
{
    QHeaderView::resizeEvent( event );
	
    if ( mProperties.proportional ) {
        int total = 0;
		int totalSize = 0;
		
        for ( int i = 0; i < count(); ++i ) {
            total += mProperties.factors.value( i, 1 );
		}
		
        for ( int i = 0; i < count() -1; ++i ) {
            qreal factor = mProperties.factors.value( i, 1 ) /qreal( total );
            int sectionSize = factor *( orientation() == Qt::Horizontal ? width() : height() );
            sectionSize = qMax( minimumSectionSize(), sectionSize );
            resizeSection( i, sectionSize );
            totalSize += sectionSize;
        }
		
        resizeSection( count() -1, width() -totalSize );
    }
}

bool pHeaderView::hasProportionalSectionSizes() const
{
    return mProperties.proportional;
}

void pHeaderView::setProportionalSectionSizes( bool enabled )
{
    if ( mProperties.proportional != enabled ) {
        mProperties.proportional = enabled;
		
        if ( enabled ) {
            setResizeMode( QHeaderView::Fixed );
		}
    }
}

int pHeaderView::sectionStretchFactor( int logicalIndex ) const
{
    return mProperties.factors.value( logicalIndex );
}

void pHeaderView::setSectionStretchFactor( int logicalIndex, int factor )
{
    mProperties.factors[ logicalIndex ] = factor;
}
