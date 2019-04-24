import pytest
import merlict as ml

@pytest.mark.xfail
def test_default_instance():
    env = ml.PropagationEnvironment()

    assert env.root_frame.this == ml.VOID_FRAME.this
    assert env.config.this == ml.DEFAULT_PROPAGATIONCONFIG.this

    # this is somehow not exposed by the wrapper ... :-(
    # therefore xfail .. the two above work.
    assert env.prng == ml.VOID_PRNG
