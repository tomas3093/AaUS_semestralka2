#include "dijkstra.h"
#include "../../ds_routines.h"
#include "../../list/array_list.h"
#include "../../stack/explicit_stack.h"

#include <stdexcept>

#pragma region Dijkstra::GraphDataDijkstraVertex

const structures::Dijkstra::GraphDataDijkstraVertex structures::Dijkstra::GraphDataDijkstraVertex::classKey_;

structures::Dijkstra::GraphDataDijkstraVertex::GraphDataDijkstraVertex() :
	GraphData(),
	predecessor_(nullptr),
	distance_(DSRoutines::getInfinity()),
	definitive_(false)
{
}

structures::Dijkstra::GraphDataDijkstraVertex::GraphDataDijkstraVertex(const GraphDataDijkstraVertex & other) :
	GraphData(other),
	predecessor_(other.predecessor_),
	distance_(other.distance_),
	definitive_(other.definitive_)
{
}

structures::GraphData & structures::Dijkstra::GraphDataDijkstraVertex::operator=(const GraphData & other)
{
	return *this = dynamic_cast<const GraphDataDijkstraVertex&>(other);
}

structures::Dijkstra::GraphDataDijkstraVertex & structures::Dijkstra::GraphDataDijkstraVertex::operator=(const GraphDataDijkstraVertex & other)
{
	if (this != &other)
	{
		predecessor_ = other.predecessor_;
		distance_ = other.distance_;
		definitive_ = other.definitive_;
	}
	return *this;
}

structures::GraphData * structures::Dijkstra::GraphDataDijkstraVertex::clone() const
{
	return new GraphDataDijkstraVertex(*this);
}

const structures::GraphData * structures::Dijkstra::GraphDataDijkstraVertex::getClassKey() const
{
	return &classKey_;
}

void structures::Dijkstra::GraphDataDijkstraVertex::reset()
{
	predecessor_ = nullptr;
	distance_ = DSRoutines::getInfinity();
	definitive_ = false;
}

structures::GraphVertex * structures::Dijkstra::GraphDataDijkstraVertex::getPredecessor()
{
	return predecessor_;
}

void structures::Dijkstra::GraphDataDijkstraVertex::setPredecessor(GraphVertex * predecessor)
{
	predecessor_ = predecessor;
}

double structures::Dijkstra::GraphDataDijkstraVertex::getDistance()
{
	return distance_;
}

void structures::Dijkstra::GraphDataDijkstraVertex::setDistance(double distance)
{
	distance_ = distance;
}

bool structures::Dijkstra::GraphDataDijkstraVertex::isDefinitive()
{
	return definitive_;
}

void structures::Dijkstra::GraphDataDijkstraVertex::setDefinitive(bool definitive)
{
	definitive_ = definitive;
}

#pragma endregion


#pragma region Dijkstra

structures::Dijkstra::Dijkstra(Graph* graph, const GraphDataLength* lengthKey) :
	graph_(graph),
	lengthKey_(lengthKey),
	vertices_(new ArrayList<GraphVertex*>()),
	beginVertex_(nullptr),
	endVertex_(nullptr),
	dijkstraDataKey_(GraphDataDijkstraVertex().getClassKey())
{
	graph_->getVertices(*vertices_);
	graph->registerData(GraphDataType::VERTEX_DATA, dijkstraDataKey_);

	for (GraphVertex* vertex : *vertices_)
	{
		vertex->setData(*dijkstraDataKey_);
	}
}

structures::Dijkstra::~Dijkstra()
{
	graph_->unregisterData(GraphDataType::VERTEX_DATA, dijkstraDataKey_);
	dijkstraDataKey_ = nullptr;

	delete vertices_;
	vertices_ = nullptr;

	graph_ = nullptr;
	beginVertex_ = nullptr;
	endVertex_ = nullptr;
}

bool structures::Dijkstra::tryCompute(int startVertexId, int stopVertexId)
{
	// inicializacia
	beginVertex_ = graph_->getVertex(startVertexId);
	endVertex_ = graph_->getVertex(stopVertexId);

	for (GraphVertex* vertex : *vertices_)
	{
		getDijkstraData(vertex)->reset();
	}

	// algoritmus
	GraphVertex* pivot = beginVertex_;
	GraphDataDijkstraVertex* pivotData = getDijkstraData(pivot);
	pivotData->setDistance(0);
	pivotData->setDefinitive(true);

	ArrayList<GraphEdge*> successors;
	while (pivot->getId() != stopVertexId)
	{
		graph_->getSuccessors(pivot->getId(), successors);
		for (GraphEdge* edge : successors)
		{
			GraphVertex* end = edge->getEndVertex();
			GraphDataDijkstraVertex* endData = getDijkstraData(end);

			if (!endData->isDefinitive() && pivotData->getDistance() + getEdgeLength(edge) < endData->getDistance())
			{
				endData->setDistance(pivotData->getDistance() + getEdgeLength(edge));
				endData->setPredecessor(pivot);
			}
		}
		successors.clear();
		pivot = findNewPivot();
		if (pivot != nullptr)
		{
			pivotData = getDijkstraData(pivot);
			pivotData->setDefinitive(true);
		}
		else
		{
			return false;
		}
	}

	return true;
}

double structures::Dijkstra::getComputedDistance()
{
	if (endVertex_ == nullptr)
	{
		throw std::logic_error("Dijkstra::getComputedDistance: Computation has not started.");
	}
	return getDijkstraData(endVertex_)->getDistance();
}

structures::List<structures::GraphEdge*>& structures::Dijkstra::getComputedPath(List<GraphEdge*>& path)
{
	if (endVertex_ == nullptr)
	{
		throw std::logic_error("Dijkstra::getComputedPath: Computation has not started.");
	}

	structures::ExplicitStack<GraphEdge*> edgeStack;
	GraphVertex * current = endVertex_;
	GraphVertex * previous = getDijkstraData(current)->getPredecessor();
	while (previous != nullptr)
	{
		edgeStack.push(graph_->getEdge(previous->getId(), current->getId()));
		current = previous;
		previous = getDijkstraData(previous)->getPredecessor();
	}

	while (!edgeStack.isEmpty())
	{
		path.add(edgeStack.pop());
	}
	return path;
}

structures::GraphVertex * structures::Dijkstra::findNewPivot()
{
	double minDistance = DSRoutines::getInfinity();
	GraphVertex* minVertex = nullptr;
	for (GraphVertex* vertex : *vertices_)
	{
		GraphDataDijkstraVertex* vertexData = getDijkstraData(vertex);
		if (!vertexData->isDefinitive() && vertexData->getDistance() < minDistance)
		{
			minDistance = vertexData->getDistance();
			minVertex = vertex;
		}
	}

	return minVertex;
}

structures::Dijkstra::GraphDataDijkstraVertex* structures::Dijkstra::getDijkstraData(GraphVertex * vertex)
{
	return dynamic_cast<GraphDataDijkstraVertex*>(&vertex->accessData(dijkstraDataKey_));
}

double structures::Dijkstra::getEdgeLength(const GraphEdge * edge) const
{
	return dynamic_cast<const GraphDataLength&>(edge->accessData(lengthKey_)).getLength();
}

#pragma endregion