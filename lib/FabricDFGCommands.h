//
// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
//

#pragma once

// #include "FabricSpliceEditorWidget.h"

#include <iostream>

#include <maya/MArgList.h>
#include <maya/MArgParser.h>
#include <maya/MPxCommand.h>

#include "FabricDFGBaseInterface.h"
#include <DFG/DFGController.h>
#include <DFG/DFGUICmd/DFGUICmds.h>
#include <DFG/DFGUICmdHandler.h>
#include <FTL/OwnedPtr.h>

class FabricDFGGetFabricVersionCommand: public MPxCommand
{
public:

  virtual const char * getName() { return "FabricCanvasGetFabricVersion"; }
  static void* creator();
  static MSyntax newSyntax();
  virtual MStatus doIt(const MArgList &args);
  virtual bool isUndoable() const { return false; }
};

class FabricDFGGetContextIDCommand: public MPxCommand
{
public:

  virtual const char * getName() { return "FabricCanvasGetContextID"; }
  static void* creator();
  static MSyntax newSyntax();
  virtual MStatus doIt(const MArgList &args);
  virtual bool isUndoable() const { return false; }
};

class FabricDFGDestroyClientCommand: public MPxCommand
{
public:

  virtual const char * getName() { return "FabricCanvasDestroyClient"; }
  static void* creator();
  static MSyntax newSyntax();
  virtual MStatus doIt(const MArgList &args);
  virtual bool isUndoable() const { return false; }
};

class FabricDFGPackageExtensionsCommand: public MPxCommand
{
public:

  virtual const char * getName() { return "FabricCanvasPackageExtensions"; }
  static void* creator();
  static MSyntax newSyntax();
  virtual MStatus doIt(const MArgList &args);
  virtual bool isUndoable() const { return false; }
};

class FabricDFGGetBindingIDCommand: public MPxCommand
{
public:

  virtual const char * getName() { return "FabricCanvasGetBindingID"; }
  static void* creator();
  static MSyntax newSyntax();
  virtual MStatus doIt(const MArgList &args);
  virtual bool isUndoable() const { return false; }
};

class FabricDFGIncrementEvalIDCommand: public MPxCommand
{
public:

  virtual const char * getName() { return "FabricCanvasIncrementEvalID"; }
  static void* creator();
  static MSyntax newSyntax();
  virtual MStatus doIt(const MArgList &args);
  virtual bool isUndoable() const { return false; }
};


class FabricCanvasProcessMelQueueCommand: public MPxCommand
{
public:

  virtual const char * getName() { return "FabricCanvasProcessMelQueue"; }
  static void* creator();
  static MSyntax newSyntax();
  virtual MStatus doIt(const MArgList &args);
  virtual bool isUndoable() const { return false; }
};

template<class MayaDFGUICmdClass, class FabricDFGUICmdClass>
class MayaDFGUICmdWrapper : public MayaDFGUICmdClass
{
  typedef MayaDFGUICmdClass Parent;

public:

  static MString GetName()
    { return FabricDFGUICmdClass::CmdName().c_str(); }

  static MCreatorFunction GetCreator()
    { return &Creator; }

  static MCreateSyntaxFunction GetCreateSyntax()
    { return &CreateSyntax; }

  virtual MString getName()
    { return GetName(); }

protected:

  static void *Creator()
  {
    return new MayaDFGUICmdWrapper;
  }

  static MSyntax CreateSyntax()
  {
    MSyntax syntax;
    Parent::AddSyntax( syntax );
    return syntax;
  }
};

class FabricDFGBaseCommand: public MPxCommand
{
protected:

  class ArgException
  {
  public:

    ArgException( MStatus status, MString const &mString )
      : m_status( status )
      , m_desc( mString )
      {}

    MStatus getStatus()
      { return m_status; }

    MString const &getDesc()
      { return m_desc; }
  
  private:

    MStatus m_status;
    MString m_desc;
  };

  virtual MString getName() = 0;

  void logError( MString const &desc )
    { displayError( getName() + ": " + desc, true ); }
};

class FabricDFGCoreCommand: public FabricDFGBaseCommand
{
public:

  virtual MStatus doIt( const MArgList &args );
  virtual MStatus undoIt();
  virtual MStatus redoIt();

  virtual bool isUndoable() const
    { return true; }

protected:

  FabricDFGCoreCommand() {}

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    ) = 0;

  static void AddSyntax( MSyntax &syntax );

private:

  FTL::OwnedPtr<FabricUI::DFG::DFGUICmd> m_dfgUICmd;
};

class FabricDFGBindingCommand : public FabricDFGCoreCommand
{
  typedef FabricDFGCoreCommand Parent;

protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args
  {
    FabricCore::DFGBinding binding;
  };

  static void GetArgs(
    MArgParser &argParser,
    Args &args
    );
};

class FabricDFGExecCommand
  : public FabricDFGBindingCommand
{
  typedef FabricDFGBindingCommand Parent;

protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString execPath;
    FabricCore::DFGExec exec;
  };

  static void GetArgs(
    MArgParser &argParser,
    Args &args
    );
};

class FabricDFGAddNodeCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;

protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QPointF pos;
  };

  static void GetArgs( MArgParser &argParser, Args &args );
};

class FabricDFGAddRefCommand
  : public FabricDFGAddNodeCommand
{
  typedef FabricDFGAddNodeCommand Parent;

protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString desiredName;
    QString varPath;
  };

  static void GetArgs( MArgParser &argParser, Args &args );
};

// -------------------------

class FabricDFGRemoveNodesCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QStringList nodeNames;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGRemoveNodesCommand,
  FabricUI::DFG::DFGUICmd_RemoveNodes
  > MayaDFGUICmd_RemoveNodes;

class FabricDFGConnectCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QStringList srcPorts;
    QStringList dstPorts;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGConnectCommand,
  FabricUI::DFG::DFGUICmd_Connect
  > MayaDFGUICmd_Connect;

class FabricDFGDisconnectCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QStringList srcPorts;
    QStringList dstPorts;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGDisconnectCommand,
  FabricUI::DFG::DFGUICmd_Disconnect
  > MayaDFGUICmd_Disconnect;

class FabricDFGAddGraphCommand
  : public FabricDFGAddNodeCommand
{
  typedef FabricDFGAddNodeCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString title;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGAddGraphCommand,
  FabricUI::DFG::DFGUICmd_AddGraph
  > MayaDFGUICmd_AddGraph;

class FabricDFGUICmdImportNodeFromJSON
  : public FabricDFGAddNodeCommand
{
  typedef FabricDFGAddNodeCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString nodeName;
    QString filePath;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGUICmdImportNodeFromJSON,
  FabricUI::DFG::DFGUICmd_ImportNodeFromJSON
  > MayaDFGUICmd_ImportNodeFromJSON;

class FabricDFGAddFuncCommand
  : public FabricDFGAddNodeCommand
{
  typedef FabricDFGAddNodeCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString title;
    QString code;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGAddFuncCommand,
  FabricUI::DFG::DFGUICmd_AddFunc
  > MayaDFGUICmd_AddFunc;

class FabricDFGInstPresetCommand
  : public FabricDFGAddNodeCommand
{
  typedef FabricDFGAddNodeCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString presetPath;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGInstPresetCommand,
  FabricUI::DFG::DFGUICmd_InstPreset
  > MayaDFGUICmd_InstPreset;

class FabricDFGAddVarCommand
  : public FabricDFGAddNodeCommand
{
  typedef FabricDFGAddNodeCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString desiredName;
    QString type;
    QString extDep;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGAddVarCommand,
  FabricUI::DFG::DFGUICmd_AddVar
  > MayaDFGUICmd_AddVar;

class FabricDFGAddGetCommand
  : public FabricDFGAddRefCommand
{
  typedef FabricDFGAddRefCommand Parent;
  
protected:

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGAddGetCommand,
  FabricUI::DFG::DFGUICmd_AddGet
  > MayaDFGUICmd_AddGet;

class FabricDFGAddSetCommand
  : public FabricDFGAddRefCommand
{
  typedef FabricDFGAddRefCommand Parent;
  
protected:

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGAddSetCommand,
  FabricUI::DFG::DFGUICmd_AddSet
  > MayaDFGUICmd_AddSet;

class FabricDFGAddPortCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString desiredPortName;
    FabricCore::DFGPortType portType;
    QString typeSpec;
    QString portToConnectWith;
    QString extDep;
    QString uiMetadata;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGAddPortCommand,
  FabricUI::DFG::DFGUICmd_AddPort
  > MayaDFGUICmd_AddPort;

class FabricDFGAddInstPortCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString instName;
    QString desiredPortName;
    FabricCore::DFGPortType portType;
    QString typeSpec;
    QString pathToConnect;
    FabricCore::DFGPortType connectType;
    QString extDep;
    QString metaData;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGAddInstPortCommand,
  FabricUI::DFG::DFGUICmd_AddInstPort
  > MayaDFGUICmd_AddInstPort;

class FabricDFGAddInstBlockPortCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString instName;
    QString blockName;
    QString desiredPortName;
    QString typeSpec;
    QString pathToConnect;
    QString extDep;
    QString metaData;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGAddInstBlockPortCommand,
  FabricUI::DFG::DFGUICmd_AddInstBlockPort
  > MayaDFGUICmd_AddInstBlockPort;

class FabricDFGCreatePresetCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString nodeName;
    QString presetDirPath;
    QString presetName;
    bool updateOrigPreset;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGCreatePresetCommand,
  FabricUI::DFG::DFGUICmd_CreatePreset
  > MayaDFGUICmd_CreatePreset;

class FabricDFGEditPortCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString oldPortName;
    QString desiredNewPortName;
    FabricCore::DFGPortType portType;
    QString typeSpec;
    QString extDep;
    QString uiMetadata;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGEditPortCommand,
  FabricUI::DFG::DFGUICmd_EditPort
  > MayaDFGUICmd_EditPort;

class FabricDFGRemovePortCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QStringList portNames;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGRemovePortCommand,
  FabricUI::DFG::DFGUICmd_RemovePort
  > MayaDFGUICmd_RemovePort;

class FabricDFGMoveNodesCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QStringList nodeNames;
    QList<QPointF> poss;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGMoveNodesCommand,
  FabricUI::DFG::DFGUICmd_MoveNodes
  > MayaDFGUICmd_MoveNodes;

class FabricDFGResizeBackDropCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString nodeName;
    QPointF xy;
    QSizeF wh;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGResizeBackDropCommand,
  FabricUI::DFG::DFGUICmd_ResizeBackDrop
  > MayaDFGUICmd_ResizeBackDrop;

class FabricDFGImplodeNodesCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QStringList nodeNames;
    QString desiredImplodedNodeName;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGImplodeNodesCommand,
  FabricUI::DFG::DFGUICmd_ImplodeNodes
  > MayaDFGUICmd_ImplodeNodes;

class FabricDFGExplodeNodeCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString node;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGExplodeNodeCommand,
  FabricUI::DFG::DFGUICmd_ExplodeNode
  > MayaDFGUICmd_ExplodeNode;

class FabricDFGAddBackDropCommand
  : public FabricDFGAddNodeCommand
{
  typedef FabricDFGAddNodeCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString title;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGAddBackDropCommand,
  FabricUI::DFG::DFGUICmd_AddBackDrop
  > MayaDFGUICmd_AddBackDrop;

class FabricDFGSetNodeCommentCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString nodeName;
    QString comment;
    bool expanded;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGSetNodeCommentCommand,
  FabricUI::DFG::DFGUICmd_SetNodeComment
  > MayaDFGUICmd_SetNodeComment;

class FabricDFGSetCodeCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString code;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGSetCodeCommand,
  FabricUI::DFG::DFGUICmd_SetCode
  > MayaDFGUICmd_SetCode;

class FabricDFGEditNodeCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString oldNodeName;
    QString desiredNewNodeName;
    QString nodeMetadata;
    QString execMetadata;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGEditNodeCommand,
  FabricUI::DFG::DFGUICmd_EditNode
  > MayaDFGUICmd_EditNode;

class FabricDFGRenamePortCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString oldPortName;
    QString desiredNewPortName;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGRenamePortCommand,
  FabricUI::DFG::DFGUICmd_RenamePort
  > MayaDFGUICmd_RenamePort;

class FabricDFGPasteCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString text;
    QPointF xy;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGPasteCommand,
  FabricUI::DFG::DFGUICmd_Paste
  > MayaDFGUICmd_Paste;

class FabricDFGSetArgValueCommand
  : public FabricDFGBindingCommand
{
  typedef FabricDFGBindingCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString argName;
    FabricCore::RTVal value;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGSetArgValueCommand,
  FabricUI::DFG::DFGUICmd_SetArgValue
  > MayaDFGUICmd_SetArgValue;

class FabricDFGSetPortDefaultValueCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString portPath;
    FabricCore::RTVal value;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGSetPortDefaultValueCommand,
  FabricUI::DFG::DFGUICmd_SetPortDefaultValue
  > MayaDFGUICmd_SetPortDefaultValue;

class FabricDFGSetRefVarPathCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString refName;
    QString varPath;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGSetRefVarPathCommand,
  FabricUI::DFG::DFGUICmd_SetRefVarPath
  > MayaDFGUICmd_SetRefVarPath;

class FabricDFGReorderPortsCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString itemPath;
    QList<int> indices;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGReorderPortsCommand,
  FabricUI::DFG::DFGUICmd_ReorderPorts
  > MayaDFGUICmd_ReorderPorts;

class FabricDFGSetExtDepsCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QStringList extDeps;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGSetExtDepsCommand,
  FabricUI::DFG::DFGUICmd_SetExtDeps
  > MayaDFGUICmd_SetExtDeps;

class FabricDFGSplitFromPresetCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QStringList extDeps;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGSplitFromPresetCommand,
  FabricUI::DFG::DFGUICmd_SplitFromPreset
  > MayaDFGUICmd_SplitFromPreset;

class FabricDFGDismissLoadDiagsCommand
  : public FabricDFGBindingCommand
{
  typedef FabricDFGBindingCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QList<int> indices;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGDismissLoadDiagsCommand,
  FabricUI::DFG::DFGUICmd_DismissLoadDiags
  > MayaDFGUICmd_DismissLoadDiags;

class FabricDFGAddBlockCommand
  : public FabricDFGAddNodeCommand
{
  typedef FabricDFGAddNodeCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString desiredName;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGAddBlockCommand,
  FabricUI::DFG::DFGUICmd_AddBlock
  > MayaDFGUICmd_AddBlock;

class FabricDFGAddBlockPortCommand
  : public FabricDFGExecCommand
{
  typedef FabricDFGExecCommand Parent;
  
protected:

  static void AddSyntax( MSyntax &syntax );

  struct Args : Parent::Args
  {
    QString blockName;
    QString desiredPortName;
    FabricCore::DFGPortType portType;
    QString typeSpec;
    QString pathToConnect;
    FabricCore::DFGPortType connectType;
    QString extDep;
    QString metaData;
  };

  static void GetArgs( MArgParser &argParser, Args &args );

  virtual FabricUI::DFG::DFGUICmd *executeDFGUICmd(
    MArgParser &argParser
    );
};
typedef MayaDFGUICmdWrapper<
  FabricDFGAddBlockPortCommand,
  FabricUI::DFG::DFGUICmd_AddBlockPort
  > MayaDFGUICmd_AddBlockPort;

// -------------------------

class FabricDFGImportJSONCommand
  : public FabricDFGBaseCommand
{
public:

  static void* creator()
    { return new FabricDFGImportJSONCommand; }

  virtual MString getName()
    { return "dfgImportJSON"; }

  static MSyntax newSyntax();
  virtual MStatus doIt( const MArgList &args );
  virtual bool isUndoable() const { return false; }
};

class FabricDFGReloadJSONCommand
  : public FabricDFGBaseCommand
{
public:

  static void* creator()
    { return new FabricDFGReloadJSONCommand; }

  virtual MString getName()
    { return "dfgReloadJSON"; }

  static MSyntax newSyntax();
  virtual MStatus doIt( const MArgList &args );
  virtual bool isUndoable() const { return false; }
};

class FabricDFGExportJSONCommand
  : public FabricDFGBaseCommand
{
public:

  static void* creator()
    { return new FabricDFGExportJSONCommand; }

  virtual MString getName()
    { return "dfgExportJSON"; }

  static MSyntax newSyntax();
  virtual MStatus doIt( const MArgList &args );
  virtual bool isUndoable() const { return false; }
};

class FabricCanvasGetExecuteSharedCommand
  : public FabricDFGBaseCommand
{
public:

  static void* creator()
    { return new FabricCanvasGetExecuteSharedCommand; }

  virtual MString getName()
    { return "FabricCanvasGetExecuteShared"; }

  static MSyntax newSyntax();
  virtual MStatus doIt( const MArgList &args );
  virtual bool isUndoable() const { return false; }
};

class FabricCanvasSetExecuteSharedCommand
  : public FabricDFGBaseCommand
{
public:

  static void* creator()
    { return new FabricCanvasSetExecuteSharedCommand; }

  virtual MString getName()
    { return "FabricCanvasSetExecuteShared"; }

  static MSyntax newSyntax();
  virtual MStatus doIt( const MArgList &args );
  virtual MStatus undoIt();
  virtual MStatus redoIt();
  virtual bool isUndoable() const { return true; }

private:

  FabricDFGBaseInterface *m_interf;
  QString m_oldMetadataValue;
  QString m_newMetadataValue;
};

class FabricCanvasReloadExtensionCommand
  : public FabricDFGBaseCommand
{
public:

  static void* creator()
    { return new FabricCanvasReloadExtensionCommand; }

  virtual MString getName()
    { return "FabricCanvasReloadExtension"; }

  static MSyntax newSyntax();
  virtual MStatus doIt( const MArgList &args );
  virtual bool isUndoable() const { return false; }
};

