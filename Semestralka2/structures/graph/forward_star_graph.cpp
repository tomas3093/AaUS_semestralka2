#include "forward_star_graph.h"
#include "../table/treap.h"
#include "../list/linked_list.h"

#include <stdexcept>

#pragma region GraphVertexForwardStar

structures::GraphVertexForwardStar::GraphVertexForwardStar(const Graph* graph, int vertexId, Table<GraphVertex*, GraphEdge*>* forwardEdges) :
	GraphVertex(graph, vertexId),
	forwardEdges_(forwardEdges)
{
}

structures::GraphVertexForwardStar::~GraphVertexForwardStar()
{
	delete forwardEdges_;
	forwardEdges_ = nullptr;
}

structures::GraphElement * structures::GraphVertexForwardStar::clone(const Graph* graph) const
{
	GraphVertexForwardStar * newStar = new GraphVertexForwardStar(*this);
	newStar->setGraph(graph);

	return newStar;
}

structures::GraphVertex & structures::GraphVertexForwardStar::operator=(const GraphVertex & other)
{
	throw std::logic_error("Assignment operator is not defined for forward star.");
}

bool structures::GraphVertexForwardStar::containsForwardEdge(GraphVertex * endVertex)
{
	return forwardEdges_->containsKey(endVertex);
}

bool structures::GraphVertexForwardStar::tryInsertForwardEdge(GraphEdge * edge)
{
	if (this == edge->getBeginVertex() && !forwardEdges_->containsKey(edge->getEndVertex()))
	{
		forwardEdges_->insert(edge->getEndVertex(), edge);
		return true;
	}
	return false;
}

bool structures::GraphVertexForwardStar::tryRemoveForwardEdge(GraphEdge * edge)
{
	if (this == edge->getBeginVertex() && forwardEdges_->containsKey(edge->getEndVertex()))
	{
		forwardEdges_->remove(edge->getEndVertex());
		return true;
	}
	return false;
}

structures::GraphEdge * structures::GraphVertexForwardStar::removeForwardEdge(GraphVertex * endVertex)
{
	return forwardEdges_->remove(endVertex);
}

structures::List<structures::GraphEdge*>& structures::GraphVertexForwardStar::removeForwardEdges(List<GraphEdge*>& list)
{
	getForwardEdges(list);
	forwardEdges_->clear();
	return list;
}

structures::GraphEdge * structures::GraphVertexForwardStar::getForwardEdge(GraphVertex * endVertex)
{
	return (*forwardEdges_)[endVertex];
}

structures::List<structures::GraphEdge*>& structures::GraphVertexForwardStar::getForwardEdges(List<GraphEdge*>& list)
{
	for (TableItem<GraphVertex*, GraphEdge*>* item : *forwardEdges_)
	{
		list.add(item->accessData());
	}
	return list;
}

structures::GraphVertexForwardStar::GraphVertexForwardStar(const GraphVertexForwardStar & other) :
	GraphVertex(other),
	forwardEdges_(dynamic_cast<Table<GraphVertex*, GraphEdge*>*>(other.forwardEdges_->clone()))
{
	forwardEdges_->clear();
}

#pragma endregion


#pragma region ForwardStarGraph

structures::ForwardStarGraph::ForwardStarGraph() :
	GraphWithRegistration(new Treap<const GraphData*, const GraphData*>(), new Treap<const GraphData*, const GraphData*>()),
	vertices_(new Treap<int, GraphVertexForwardStar*>())
{
}

structures::ForwardStarGraph::ForwardStarGraph(const ForwardStarGraph & other) :
	ForwardStarGraph()
{
	*this = other;
}

structures::ForwardStarGraph::~ForwardStarGraph()
{
	clear();
	delete vertices_;
	vertices_ = nullptr;
}

structures::Structure * structures::ForwardStarGraph::clone() const
{
	return new ForwardStarGraph(*this);
}

structures::GraphWithRegistration & structures::ForwardStarGraph::operator=(const GraphWithRegistration & other)
{
	return *this = dynamic_cast<const ForwardStarGraph&>(other);
}

structures::ForwardStarGraph & structures::ForwardStarGraph::operator=(const ForwardStarGraph & other)
{
	if (this != &other)
	{
		clear();

		GraphWithRegistration::operator=(other);

		for (TableItem<int, GraphVertexForwardStar*>* item : *other.vertices_)
		{
			GraphVertexForwardStar* newStar = dynamic_cast<GraphVertexForwardStar*>(item->accessData()->clone(this));
			vertices_->insert(newStar->getId(), newStar);
		}

		for (TableItem<int, GraphVertexForwardStar*>* item : *other.vertices_)
		{
			GraphVertexForwardStar* otherBeginStar = item->accessData();
			GraphVertexForwardStar* newBeginStar = (*vertices_)[otherBeginStar->getId()];

			LinkedList<GraphEdge*> otherEdges;
			otherBeginStar->getForwardEdges(otherEdges);
			for (GraphEdge* otherEdge : otherEdges)
			{
				GraphVertexForwardStar* newEndStar = (*vertices_)[otherEdge->getEndVertex()->getId()];
				GraphEdge* newEdge = dynamic_cast<GraphEdge*>(otherEdge)->clone(newBeginStar, newEndStar);
				if (!newBeginStar->tryInsertForwardEdge(newEdge))
				{
					throw std::logic_error("ForwardStarGraph::operator=: Unexpected error.");
				}
			}
		}
	}
	return *this;
}

size_t structures::ForwardStarGraph::size() const
{
	return vertices_->size();
}

void structures::ForwardStarGraph::clear()
{
	for (TableItem<int, GraphVertexForwardStar*>* item : *vertices_)
	{
		GraphVertexForwardStar* star = item->accessData();

		LinkedList<GraphEdge*> forwardEdges;
		star->removeForwardEdges(forwardEdges);
		for (GraphEdge* edge : forwardEdges)
		{
			delete edge;
		}
		delete star;
	}
	vertices_->clear();
}

structures::GraphVertex* structures::ForwardStarGraph::createVertex(int vertexId)
{
	if (!vertices_->containsKey(vertexId))
	{
		GraphVertexForwardStar *vertex = new GraphVertexForwardStar(this, vertexId, new Treap<GraphVertex*, GraphEdge*>());
		vertices_->insert(vertex->getId(), vertex);
		return vertex;
	}
	else
	{
		throw std::logic_error("ForwardStarGraph::createVertex: Vertex with given id already exists in graph.");
	}
}

void structures::ForwardStarGraph::removeVertex(int vertexId)
{
	GraphVertexForwardStar *vertex = vertices_->remove(vertexId);

	LinkedList<GraphEdge*> accessibleEdges;
	vertex->removeForwardEdges(accessibleEdges);
	for (GraphEdge* edge : accessibleEdges)
	{
		delete edge;
	}
	accessibleEdges.clear();

	for (TableItem<int, GraphVertexForwardStar*>* item : *vertices_)
	{
		GraphVertexForwardStar* star = item->accessData();

		if (star->containsForwardEdge(vertex))
		{
			GraphEdge* edge = star->removeForwardEdge(vertex);
			delete edge;
		}
	}

	delete vertex;
}

void structures::ForwardStarGraph::removeVertex(GraphVertex * vertex)
{
	removeVertex(vertex->getId());
}

structures::GraphEdge* structures::ForwardStarGraph::createEdge(int beginVertexId, int endVertexId)
{
	GraphVertexForwardStar *begin = (*vertices_)[beginVertexId];
	GraphVertexForwardStar *end = (*vertices_)[endVertexId];

	GraphEdge *edge = new GraphEdge(begin, end);
	if (!begin->tryInsertForwardEdge(edge))
	{
		delete edge;
		throw std::logic_error("ForwardStarGraph::createEdge: Edge cannot be inserted.");
	}
	return edge;
}

void structures::ForwardStarGraph::removeEdge(int beginVertexId, int endVertexId)
{
	GraphVertexForwardStar *begin = (*vertices_)[beginVertexId];
	GraphVertexForwardStar *end = (*vertices_)[endVertexId];

	GraphEdge* edge = begin->removeForwardEdge(end);
	delete edge;
}

void structures::ForwardStarGraph::removeEdge(GraphEdge * edge)
{
	removeEdge(edge->getBeginVertex()->getId(), edge->getEndVertex()->getId());
}

structures::GraphVertex * structures::ForwardStarGraph::getVertex(int vertexId)
{
	return (*vertices_)[vertexId];
}

structures::List<structures::GraphVertex*>& structures::ForwardStarGraph::getVertices(List<GraphVertex*>& list)
{
	for (TableItem<int, GraphVertexForwardStar*>* item : *vertices_)
	{
		list.add(item->accessData());
	}
	return list;
}

structures::GraphEdge * structures::ForwardStarGraph::getEdge(int beginVertexId, int endVertexId)
{
	GraphVertexForwardStar *begin = (*vertices_)[beginVertexId];
	GraphVertexForwardStar *end = (*vertices_)[endVertexId];

	return begin->getForwardEdge(end);
}

structures::List<structures::GraphEdge*>& structures::ForwardStarGraph::getEdges(List<GraphEdge*>& list)
{
	for (TableItem<int, GraphVertexForwardStar*>* item : *vertices_)
	{
		item->accessData()->getForwardEdges(list);
	}
	return list;
}

structures::List<structures::GraphEdge*>& structures::ForwardStarGraph::getSuccessors(int vertexId, List<GraphEdge*>& list)
{
	GraphVertexForwardStar *vertex = (*vertices_)[vertexId];

	vertex->getForwardEdges(list);
	return list;
}

structures::List<structures::GraphEdge*>& structures::ForwardStarGraph::getPredecessors(int vertexId, List<GraphEdge*>& list)
{
	GraphVertexForwardStar *vertex = (*vertices_)[vertexId];

	for (TableItem<int, GraphVertexForwardStar*>* item : *vertices_)
	{
		GraphVertexForwardStar* star = item->accessData();

		if (star->containsForwardEdge(vertex))
		{
			GraphEdge* edge = star->getForwardEdge(vertex);
			list.add(edge);
		}
	}

	return list;
}

#pragma endregion