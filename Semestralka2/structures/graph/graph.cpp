#include "graph.h"
#include <stdexcept>
#include "../table/sorted_sequence_table.h"
#include "../list/linked_list.h"

#pragma region GraphData

structures::GraphData::~GraphData()
{
}

structures::GraphData::GraphData()
{
}

structures::GraphData::GraphData(const GraphData & other)
{
}

#pragma endregion


#pragma region GraphDataLength

const structures::GraphDataLength structures::GraphDataLength::classKey_;

structures::GraphDataLength::GraphDataLength() :
	GraphData(),
	length_(0)
{
}

structures::GraphDataLength::GraphDataLength(const GraphDataLength & other) :
	GraphData(other),
	length_(other.length_)
{
}

structures::GraphData & structures::GraphDataLength::operator=(const GraphData & other)
{
	return *this = dynamic_cast<const GraphDataLength&>(other);
}

structures::GraphData * structures::GraphDataLength::clone() const
{
	return new GraphDataLength(*this);
}

const structures::GraphData * structures::GraphDataLength::getClassKey() const
{
	return &classKey_;
}

structures::GraphDataLength & structures::GraphDataLength::operator=(const GraphDataLength & other)
{
	if (this != &other)
	{
		length_ = other.length_;
	}
	return *this;
}

double structures::GraphDataLength::getLength() const
{
	return length_;
}

void structures::GraphDataLength::setLength(double price)
{
	length_ = price;
}

#pragma endregion


#pragma region GraphElement

structures::GraphElement::GraphElement(const Graph* graph, const GraphDataType dataType) :
	dataType_(dataType),
	graph_(graph),
	dataTable_(new SortedSequenceTable<const GraphData*, GraphData*>())
{
}

structures::GraphElement::GraphElement(const GraphElement & other) :
	GraphElement(other.graph_, other.dataType_)
{
	*this = other;
}

structures::GraphElement::~GraphElement()
{
	removeAllData();
	
	delete dataTable_;
	dataTable_ = nullptr;

	graph_ = nullptr;
}

structures::GraphElement & structures::GraphElement::operator=(const GraphElement & other)
{
	if (this != &other)
	{
		graph_ = other.graph_;
		dataType_ = other.dataType_;

		removeAllData();
		for (TableItem<const GraphData*, GraphData*>* item : *other.dataTable_)
		{
			dataTable_->insert(item->getKey(), item->accessData()->clone());
		}
	}
	return *this;
}

void structures::GraphElement::setData(const GraphData & data)
{
	GraphData* curData = nullptr;
	const GraphData * key = data.getClassKey();
	if (dataTable_->tryFind(key, curData) || graph_->areDataRegistered(dataType_, key))
	{
		if (curData != nullptr)
		{
			delete curData;
			(*dataTable_)[key] = data.clone();
		}
		else
		{
			dataTable_->insert(key, data.clone());
		}
	}
	else
	{
		throw std::logic_error("GraphElement::setData: Data with given dataKey are not registered in graph for this GraphElement.");
	}
}

void structures::GraphElement::removeData(const GraphData * key)
{
	GraphData* data = dataTable_->remove(key->getClassKey());
	delete data;
}

void structures::GraphElement::removeAllData()
{
	for (TableItem<const GraphData*, GraphData*>* item : *dataTable_)
	{
		delete item->accessData();
	}
	dataTable_->clear();
}

structures::GraphData & structures::GraphElement::accessData(const GraphData * key)
{
	GraphData* curData = nullptr;
	if (dataTable_->tryFind(key->getClassKey(), curData))
	{
		return *curData;
	}
	throw std::logic_error("GraphElement::accessData: Data are not available.");
}

const structures::GraphData & structures::GraphElement::accessData(const GraphData * key) const
{
	GraphData* curData = nullptr;
	if (dataTable_->tryFind(key->getClassKey(), curData))
	{
		return *curData;
	}
	throw std::logic_error("GraphElement::accessData: Data are not available.");
}

bool structures::GraphElement::containsData(const GraphData * key) const
{
	GraphData* curData = nullptr;
	return dataTable_->tryFind(key->getClassKey(), curData);
}

const structures::Graph * structures::GraphElement::getGraph() const
{
	return graph_;
}

void structures::GraphElement::setGraph(const Graph * graph)
{
	if (graph_ != graph)
	{
		LinkedList<const GraphData*> dataKeyList;
		for (TableItem<const GraphData*, GraphData*>* item : *dataTable_)
		{
			if (!graph->areDataRegistered(dataType_, item->getKey()))
			{
				dataKeyList.add(item->getKey());
			}
		}

		for (const GraphData* dataKey : dataKeyList)
		{
			removeData(dataKey);
		}

		graph_ = graph;
	}
}

#pragma endregion


#pragma region GraphVertex

structures::GraphVertex::GraphVertex(const Graph* graph, int id) :
	GraphElement(graph, GraphDataType::VERTEX_DATA),
	id_(id)
{
}

structures::GraphVertex::GraphVertex(const GraphVertex & other) :
	GraphElement(other),
	id_(other.id_)
{
}

structures::GraphElement * structures::GraphVertex::clone(const Graph* graph) const
{
	GraphVertex * newVertex = new GraphVertex(*this);
	newVertex->setGraph(graph);

	return newVertex;
}

structures::GraphElement & structures::GraphVertex::operator=(const GraphElement & other)
{
	return *this = dynamic_cast<const GraphVertex&>(other);
}

structures::GraphVertex & structures::GraphVertex::operator=(const GraphVertex & other)
{
	if (this != &other)
	{
		GraphElement::operator=(other);
		id_ = other.id_;
	}
	return *this;
}

int structures::GraphVertex::getId() const
{
	return id_;
}

#pragma endregion


#pragma region GraphEdge

structures::GraphEdge::GraphEdge(GraphVertex * beginVertex, GraphVertex * endVertex) :
	GraphElement(beginVertex->getGraph(), GraphDataType::EDGE_DATA),
	beginVertex_(beginVertex),
	endVertex_(endVertex)
{
	if (beginVertex_->getGraph() != endVertex_->getGraph())
	{
		throw std::logic_error("GraphEdge::GraphEdge: beginVertex and endVertex belong to different graphs.");
	}
}

structures::GraphEdge::GraphEdge(const GraphEdge & other) :
	GraphElement(other),
	beginVertex_(other.beginVertex_),
	endVertex_(other.endVertex_)
{
}

structures::GraphEdge * structures::GraphEdge::clone(GraphVertex * newBeginVertex, GraphVertex * newEndVertex) const
{
	if (newBeginVertex->getGraph() == newEndVertex->getGraph())
	{
		GraphEdge* newEdge = new GraphEdge(*this);
		newEdge->beginVertex_ = newBeginVertex;
		newEdge->endVertex_ = newEndVertex;
		newEdge->setGraph(newBeginVertex->getGraph());

		return newEdge;
	}
	throw std::logic_error("GraphEdge::clone: newBeginVertex and newEndVertex belong to different graphs.");
}

structures::GraphElement & structures::GraphEdge::operator=(const GraphElement & other)
{
	return *this = dynamic_cast<const GraphEdge&>(other);
}

structures::GraphEdge & structures::GraphEdge::operator=(const GraphEdge & other)
{
	if (this != &other)
	{
		GraphElement::operator=(other);
		beginVertex_ = other.beginVertex_;
		endVertex_ = other.endVertex_;
	}
	return *this;
}

structures::GraphVertex * structures::GraphEdge::getBeginVertex()
{
	return beginVertex_;
}

const structures::GraphVertex * structures::GraphEdge::getBeginVertex() const
{
	return beginVertex_;
}

structures::GraphVertex * structures::GraphEdge::getEndVertex()
{
	return endVertex_;
}

const structures::GraphVertex * structures::GraphEdge::getEndVertex() const
{
	return endVertex_;
}

structures::Structure & structures::Graph::operator=(const Structure & other)
{
	return *this = dynamic_cast<const Graph&>(other);
}

#pragma endregion


#pragma region Graph

structures::Graph::Graph()
{
}

#pragma endregion
