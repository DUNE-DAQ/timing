from __future__ import print_function

import pdt.cli.toolbox as toolbox

# ------------------------------------------------------------------------------
class ShellFactory(object):
    """docstring for Factory"""

    __factoryregister__ = {
        'board': {},
        'design': {},
    }

    @classmethod
    def registerBoard(cls, aId, aCls):
        if aId in cls.__factoryregister__['board']:
            raise RuntimeError('Board '+aId+' already registered')

        cls.__factoryregister__['board'][aId] = aCls
    
    @classmethod
    def registerDesign(cls, aId, aCls):
        if aId in cls.__factoryregister__['design']:
            raise RuntimeError('Design '+aId+' already registered')

        cls.__factoryregister__['design`'][aId] = aCls

    @classmethod
    def make(cls, device):
        lBoardInfo = toolbox.readSubNodes(device.getNode('io.config'), False)
        device.dispatch()
        boardType = lBoardInfo['board_type'].value()
        carrierType = lBoardInfo['carrier_type'].value()
        designType = lBoardInfo['design_type'].value()

        boardcls = cls.makeBoardShell(boardType)
        designcls = cls.makeBoardShell(designType)

        if not boardcls:
            raise RuntimeError('Device '+device.id()+': Unknown board type')
        if not designcls:
            raise RuntimeError('Device '+device.id()+': Unknown design type')

        def ctor(self, device, boardType, carrierType, designType):
           self.mDevice = device
           self.mBoardType = boardType
           self.mCarrierType = carrierType
           self.mDesignType = designType

           super(self.__class__, self).__init__()

        # lCls = classmaker()(device.id(), (boardcls, designcls), {'__init__':ctor})
        lCls = type(device.id(), (boardcls, designcls), {'__init__':ctor})

        return lCls(device, boardType, carrierType, designType)

    @classmethod
    def makeBoardShell(cls, boardType):
        return cls.__factoryregister__.get('board').get(boardType,None)

    @classmethod
    def makeDesignShell(cls, designType):
        return cls.__factoryregister__.get('design').get(designType,None)
# ------------------------------------------------------------------------------
