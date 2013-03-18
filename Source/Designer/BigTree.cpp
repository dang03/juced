/*
  ==============================================================================

    BigTree.cpp
    Created: 11 Mar 2013 8:38:49am
    Author:  admin

  ==============================================================================
*/

#include "Globals.h"
#include "Constructor.h"
#include "../Modules/juced_Label.h"
#include "../Modules/juced_TextButton.h"
#include "../Modules/juced_Window.h"
#include "../Modules/juced_MainComponent.h"

class BigTree : public ValueTree,
				public ValueTree::Listener
{
public:
    //==============================================================================
	BigTree() : ValueTree()	//Modules::Unknown.toString())
	{

	}

	BigTree(const ValueTree &other) : ValueTree(other)
	{

	}

	BigTree(DynamicObject *object, var _type) : ValueTree(_type.toString()) {
		getDynamicObjectProperties(object);
		addListener(this);
	}

	BigTree(DynamicObject *object) : ValueTree(Modules::Unknown.toString())
	{
		getDynamicObjectProperties(object);
		addListener(this);
	}

	void valueTreePropertyChanged (ValueTree &treeWhosePropertyHasChanged, const Identifier &property)
	{
		//Deal with changes on this ValueTree only, not their childs.
		if (treeWhosePropertyHasChanged == *this) {

			jassert (this->hasProperty(Attributes::objectType));	//Object requires: setProperty(Ids::objectType, "object named type")

			//since Windows don't inherit from Component class we deal with it separately
			if (this->getProperty(Attributes::objectType) == Modules::Window) {
				juced_Window *obj = (juced_Window *)this->getProperty(Attributes::object).getDynamicObject();
				if (property == Attributes::height) {
					int height = treeWhosePropertyHasChanged.getProperty(property);
					obj->setSize(obj->getWidth(), height);
				} else if (property == Attributes::width) {
					int width = treeWhosePropertyHasChanged.getProperty(property);
					obj->setSize(width, obj->getHeight());
				} else if (property == Attributes::name) {
					obj->setName(treeWhosePropertyHasChanged.getProperty(property).toString());
				} else if (property == Attributes::backgroundColour) {
					obj->setName(treeWhosePropertyHasChanged.getProperty(property).toString());
					Colour newColour = Colour::fromString(treeWhosePropertyHasChanged.getProperty(property).toString());
					obj->setColour(DocumentWindow::backgroundColourId, newColour);
				}
			} else {
				//deal with other type of components since its not a window
				bool propertyChanged = true;
				if (this->getProperty(Attributes::objectType) == Modules::Label) {
					//deal with Label specific properties
					juced_Label *cObject = (juced_Label *)this->getProperty(Attributes::object).getDynamicObject();
					if (property == Attributes::text) {
						cObject->setText(treeWhosePropertyHasChanged.getProperty(property).toString(), false);
					} else if (property == Attributes::backgroundColour) {
						Colour newColour = Colour::fromString(treeWhosePropertyHasChanged.getProperty(property).toString());
						cObject->setColour(Label::backgroundColourId, newColour);
					} else if (property == Attributes::textColour) {
						Colour newColour = Colour::fromString(treeWhosePropertyHasChanged.getProperty(property).toString());
						cObject->setColour(Label::textColourId, newColour);
					} else {
						propertyChanged = false;
					}
				} else if (this->getProperty(Attributes::objectType) == Modules::TextButton) {
					//deal with TextButton specific properties
					juced_TextButton *cObject = (juced_TextButton *)this->getProperty(Attributes::object).getDynamicObject();
					if (property == Attributes::buttonText) {
						cObject->setButtonText(treeWhosePropertyHasChanged.getProperty(property).toString());
					} else if (property == Attributes::buttonColour) {
						Colour newColour = Colour::fromString(treeWhosePropertyHasChanged.getProperty(property).toString());
						cObject->setColour(TextButton::buttonColourId, newColour);
					} else if (property == Attributes::buttonOnColour) {
						Colour newColour = Colour::fromString(treeWhosePropertyHasChanged.getProperty(property).toString());
						cObject->setColour(TextButton::buttonOnColourId, newColour);
					} else {
						propertyChanged = false;
					}
				} else if (this->getProperty(Attributes::objectType) == Modules::Component) {
					//deal with Component specific properties (obviously is empty)
					propertyChanged = false;
				}
				if (!propertyChanged) {
					//deal with common component properties
					Component *obj = dynamic_cast<Component *> (this->getProperty(Attributes::object).getDynamicObject());
					if (property == Attributes::x) {
						int value = treeWhosePropertyHasChanged.getProperty(property);
						obj->setBounds(value, obj->getY(), obj->getWidth(), obj->getHeight());
					} else if (property == Attributes::y) {
						int value = treeWhosePropertyHasChanged.getProperty(property);
						obj->setBounds(obj->getX(), value, obj->getWidth(), obj->getHeight());
					} else if (property == Attributes::width) {
						int value = treeWhosePropertyHasChanged.getProperty(property);
						obj->setBounds(obj->getX(), obj->getY(), value, obj->getHeight());
					} else if (property == Attributes::height) {
						int value = treeWhosePropertyHasChanged.getProperty(property);
						obj->setBounds(obj->getX(), obj->getY(), obj->getWidth(), value);
					} else if (property == Attributes::name) {
						obj->setName(treeWhosePropertyHasChanged.getProperty(property).toString());
					}
				}
			}
			//Set current transaction description of UndoManager and start a new one
			Attribute *t = Constructor::getInstance()->getAttributeOf(property);
			Constructor::getInstance()->getUndoManager()->setCurrentTransactionName("Set " + t->display + " on " + getProperty(Attributes::name).toString());
			Constructor::getInstance()->getUndoManager()->beginNewTransaction();
		}
	}

    void valueTreeChildAdded (ValueTree& parentTree, ValueTree& childWhichHasBeenAdded)
	{
		if (parentTree == childWhichHasBeenAdded) {}	//useless - to avoid warnings
	}

    void valueTreeChildRemoved (ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved)
	{
		if (parentTree == childWhichHasBeenRemoved) {}	//useless - to avoid warnings
	}


	void valueTreeChildOrderChanged (ValueTree& parentTreeWhoseChildrenHaveMoved)
	{
		if (*this == parentTreeWhoseChildrenHaveMoved) {}	//useless - to avoid warnings
	}

	void valueTreeParentChanged (ValueTree& treeWhoseParentHasChanged)
	{
		if (*this == treeWhoseParentHasChanged) {}	//useless - to avoid warnings
	}

	void valueTreeRedirected (ValueTree& treeWhichHasBeenChanged)
	{
		if (*this == treeWhichHasBeenChanged) {}	//useless - to avoid warnings
	}
	
	BigTree& getChildWithProperty(const Identifier &propertyName, const var &propertyValue, bool recursive)
	{
		//checking root node first
		if (hasProperty(propertyName))
			if (getProperty(propertyName) == propertyValue)
				return *this;

		BigTree bTree(ValueTree::getChildWithProperty(propertyName, propertyValue));
		if (bTree.isValid()) {
			return bTree;
		} else if (!recursive) {
			return (BigTree());
		}

		for (int i = getNumChildren(); --i >= 0;) {
			BigTree cTree(getChild(i).getChildWithProperty(propertyName, propertyValue, recursive));
			if (cTree.isValid()) {
				return cTree;
			}
		}
		return (BigTree());
	}

	/*
	ValueTree getChildWithProperty (const Identifier& propertyName, const var& propertyValue) const
    {
        for (int i = 0; i < children.size(); ++i)
        {
            SharedObject* const s = children.getObjectPointerUnchecked (i);
            if (s->getProperty (propertyName) == propertyValue)
                return ValueTree (s);
        }

        return ValueTree::invalid;
    }*/

	BigTree getChild(int index) const
	{
		return BigTree(ValueTree::getChild(index));
	}

	void recursive_removeProperty(Identifier name, UndoManager *undoManager) {

		removeProperty(name, undoManager);
		for (int i = getNumChildren(); --i >= 0;) {
			getChild(i).recursive_removeProperty(name, undoManager);
		}
	}

	XmlElement* createXml()
	{

		BigTree *printableTree = new BigTree(createCopy());
		printableTree->recursive_removeProperty(Attributes::object, 0);
		ValueTree *vTree = (ValueTree *) printableTree;
		return vTree->createXml();
	}

private:

	void getDynamicObjectProperties(DynamicObject *object) {
		setProperty(Attributes::object, var(object), 0);

		NamedValueSet values = object->getProperties();
		for (int i = values.size(); --i >= 0;) {
			setProperty(values.getName(i), values.getValueAt(i), 0);
		}
		
	}
};


