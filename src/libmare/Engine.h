
#pragma once

#include "Tools/String.h"
#include "Tools/List.h"
#include "Tools/Map.h"
#include "Tools/Scope.h"

class Namespace;
class Word;
class Statement;

class Engine : public Scope
{
public:

  typedef void (*ErrorHandler)(void* userData, const String& file, int line, const String& message);

  Engine(ErrorHandler errorHandler, void* userData) : errorHandler(errorHandler), errorUserData(userData), rootStatement(0), currentSpace(0) {}

  bool load(const String& file);
  void error(const String& message);

  bool hasKey(const String& key, bool allowInheritance = true);

  bool enterKey(const String& key, bool allowInheritance = true);
  void enterUnnamedKey();
  void enterNewKey(const String& key);
  void enterRootKey();

  String getKeyOrigin(const String& key);

  bool leaveKey();
  
  void getKeys(List<String>& keys);
  bool getKeys(const String& key, List<String>& keys, bool allowInheritance = true);
  String getFirstKey();
  String getFirstKey(const String& key, bool allowInheritance = true);
  void getText(List<String>& text);
  bool getText(const String& key, List<String>& text, bool allowInheritance = true);
  String getMareDir() const;

  void addDefaultKey(const String& key);
  void addDefaultKey(const String& key, const String& value);
  void addDefaultKey(const String& key, const Map<String, String>& value);
  void addCommandLineKey(const String& key, const String& value);
  
  void pushAndLeaveKey(); // TODO: hide these functions
  bool popKey();

private:
  ErrorHandler errorHandler;
  void* errorUserData;
  Statement* rootStatement;
  Namespace* currentSpace;
  List<Namespace*> stashedKeys;

  bool resolveScript(const String& key, Word*& word, Namespace*& result);
  bool resolveScript(const String& key, Namespace* excludeStatements, Word*& word, Namespace*& result);
  void setKey(const Word& key);
  void appendKeys(String& output);
  
  friend class ReferenceStatement; // hack?
  friend class IfStatement; // hack?
  friend class Namespace;
};
