#ifndef PHEADERVIEW_H
#define PHEADERVIEW_H

#include <QHeaderView>

class pHeaderView : public QHeaderView
{
	Q_OBJECT
	
public:
	struct Properties
	{
		Properties() : proportional( false ) {}
		
		bool proportional;
		QHash<int, int> factors;
	};
	
	pHeaderView( Qt::Orientation orientation, QWidget* parent = 0 );
	
	virtual void resizeEvent( QResizeEvent* event );
	
	bool hasProportionalSectionSizes() const;
	void setProportionalSectionSizes( bool enabled );
	
	int sectionStretchFactor( int logicalIndex ) const;
	void setSectionStretchFactor( int logicalIndex, int factor );

protected:
	pHeaderView::Properties mProperties;
};

#endif // PHEADERVIEW_H
