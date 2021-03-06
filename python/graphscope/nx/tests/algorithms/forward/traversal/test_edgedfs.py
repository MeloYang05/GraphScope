import networkx.algorithms.traversal.tests.test_edgedfs
import pytest

from graphscope.nx.utils.compat import import_as_graphscope_nx
from graphscope.nx.utils.compat import with_graphscope_nx_context

import_as_graphscope_nx(networkx.algorithms.traversal.tests.test_edgedfs,
                        decorators=pytest.mark.usefixtures("graphscope_session"))

from networkx.algorithms.traversal.tests.test_edgedfs import TestEdgeDFS


@pytest.mark.usefixtures("graphscope_session")
@with_graphscope_nx_context(TestEdgeDFS)
class TestEdgeDFS():
    @pytest.mark.skip(reason="not support multigraph")
    def test_multigraph(self):
        pass

    @pytest.mark.skip(reason="not support multigraph")
    def test_multidigraph(self):
        pass

    @pytest.mark.skip(reason="not support multigraph")
    def test_multidigraph_rev(self):
        pass

    @pytest.mark.skip(reason="not support multigraph")
    def test_multidigraph_ignore(self):
        pass
