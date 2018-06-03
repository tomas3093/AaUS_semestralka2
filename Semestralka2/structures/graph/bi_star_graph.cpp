#include "bi_star_graph.h"
#include "../table/treap.h"
#include "../list/linked_list.h"

#pragma region GraphVertexBiStar

structures::GraphVertexBiStar::GraphVertexBiStar(const Graph* graph, int vertexId, Table<GraphVertex*, GraphEdge*>* forwardEdges, Table<GraphVertex*, GraphEdge*>* backwardEdges) :
	GraphVertexForwardStar(graph, vertexId, forwardEdges),
	backwardEdges_(backwardEdges)
{
}

structures::GraphVertexBiStar::~GraphVertexBiStar()
{
	delete backwardEdges_;
	backwardEdges_ = nullptr;
}

structures::GraphElement * structures::GraphVertexBiStar::clone(const Graph* graph) const
{
	GraphVertexBiStar * newStar = new GraphVertexBiStar(*this);
	newStar->setGraph(graph);

	return newStar;
}

bool structures::GraphVertexBiStar::containsBackwardEdge(GraphVertex * beginVertex)
{
	return backwardEdges_->containsKey(beginVertex);
}

bool structures::GraphVertexBiStar::tryInsertBackwardEdge(GraphEdge * edge)
{
	if (this == edge->getEndVertex() && !backwardEdges_->containsKey(edge->getBeginVertex()))
	{
		backwardEdges_->insert(edge->getBeginVertex(), edge);
		return true;
	}
	return false;
}

bool structures::GraphVertexBiStar::tryRemoveBackwardEdge(GraphEdge * edge)
{
	if (this == edge->getEndVertex() && backwardEdges_->containsKey(edge->getBeginVertex()))
	{
		backwardEdges_->remove(edge->getBeginVertex());
		return true;
	}
	return false;
}

structures::GraphEdge * structures::GraphVertexBiStar::removeBackwardEdge(GraphVertex * beginVertex)
{
	return backwardEdges_->remove(beginVertex);
}

structures::List<structures::GraphEdge*>& structures::GraphVertexBiStar::removeBackwardEdges(List<GraphEdge*>& list)
{
	getBackwardEdges(list);
	backwardEdges_->clear();
	return list;
}

structures::GraphEdge * structures::GraphVertexBiStar::getBackwardEdge(GraphVertex * beginVertex)
{
	return (*backwardEdges_)[beginVertex];
}

structures::List<structures::GraphEdge*>& structures::GraphVertexBiStar::getBackwardEdges(List<GraphEdge*>& list)
{
	for (TableItem<GraphVertex*, GraphEdge*>* item : *backwardEdges_)
	{
		list.add(item->accessData());
	}
	return list;
}

structures::GraphVertexBiStar::GraphVertexBiStar(const GraphVertexBiStar & other) :
	GraphVertexForwardStar(other),
	backwardEdges_(dynamic_cast<Table<GraphVertex*, GraphEdge*>*>(other.backwardEdges_->clone()))
{
	backwardEdges_->clear();
}

#pragma endregion


#pragma region BiStarGraph

structures::BiStarGraph::BiStarGraph() :
	ForwardStarGraph()
{
}

structures::BiStarGraph::BiStarGraph(const BiStarGraph & other) :
	BiStarGraph()
{
	*this = other;
}

structures::Structure * structures::BiStarGraph::clone() const
{
	return new BiStarGraph(*this);
}

structures::ForwardStarGraph & structures::BiStarGraph::operator=(const ForwardStarGraph & other)
{
	return *this = dynamic_cast<const BiStarGraph&>(other);
}

structures::BiStarGraph & structures::BiStarGraph::operator=(const BiStarGraph & other)
{
	if (this != &other)
	{
		ForwardStarGraph::operator=(other);
		
		LinkedList<GraphEdge*> edgeList;
		getEdges(edgeList);
		for (GraphEdge* edge : edgeList)
		{
			GraphVertexBiStar *end = dynamic_cast<GraphVertexBiStar*>(edge->getEndVertex());
			if (!end->tryInsertBackwardEdge(edge))
			{
				/* Tento pripad by nikdy nemal nastat. */
				throw std::logic_error("BiStarGraph::operator=: Unexpected error.");
			}
		}
	}
	return *this;
}

structures::GraphVertex * structures::BiStarGraph::createVertex(int vertexId)
{
	if (!vertices_->containsKey(vertexId))
	{
		GraphVertexBiStar *vertex = new GraphVertexBiStar(this, vertexId, new Treap<GraphVertex*, GraphEdge*>(), new Treap<GraphVertex*, GraphEdge*>());
		vertices_->insert(vertex->getId(), vertex);
		return vertex;
	}
	else
	{
		throw std::logic_error("BiStarGraph::createVertex: Vertex with given id already exists in graph.");
	}
}

void structures::BiStarGraph::removeVertex(int vertexId)
{
	GraphVertexBiStar *vertex = dynamic_cast<GraphVertexBiStar*>(vertices_->remove(vertexId));

	LinkedList<GraphEdge*> accessibleEdges;
	vertex->removeForwardEdges(accessibleEdges);
	for (GraphEdge* item : accessibleEdges)
	{
		GraphEdge* edge = dynamic_cast<GraphVertexBiStar*>(item->getEndVertex())->removeBackwardEdge(vertex);
		delete edge;
	}
	accessibleEdges.clear();

	vertex->removeBackwardEdges(accessibleEdges);
	for (GraphEdge* item : accessibleEdges)
	{
		GraphEdge* edge = dynamic_cast<GraphVertexBiStar*>(item->getBeginVertex())->removeForwardEdge(vertex);
		delete edge;
	}
	accessibleEdges.clear();

	delete vertex;
}

structures::GraphEdge* structures::BiStarGraph::createEdge(int beginVertexId, int endVertexId)
{
	GraphEdge *edge = ForwardStarGraph::createEdge(beginVertexId, endVertexId);
	GraphVertexBiStar *end = dynamic_cast<GraphVertexBiStar*>(edge->getEndVertex());

	if (!end->tryInsertBackwardEdge(edge))
	{
		/* Tento pripad by nikdy nemal nastat. */
		GraphVertexBiStar *begin = dynamic_cast<GraphVertexBiStar*>(edge->getBeginVertex());
		begin->removeForwardEdge(end);
		delete edge;
		throw std::logic_error("BiStarGraph::createEdge: Unexpected error.");
	}
	return edge;
}

void structures::BiStarGraph::removeEdge(int beginVertexId, int endVertexId)
{
	GraphVertexForwardStar *begin = (*vertices_)[beginVertexId];
	GraphVertexBiStar *end = dynamic_cast<GraphVertexBiStar*>((*vertices_)[endVertexId]);

	begin->removeForwardEdge(end);
	GraphEdge* edge = end->removeBackwardEdge(begin);
	delete edge;
}

structures::List<structures::GraphEdge*>& structures::BiStarGraph::getPredecessors(int vertexId, List<GraphEdge*>& list)
{
	GraphVertexBiStar *vertex = dynamic_cast<GraphVertexBiStar*>((*vertices_)[vertexId]);

	vertex->getBackwardEdges(list);
	return list;
}

#pragma endregion
