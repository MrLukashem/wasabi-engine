
#pragma once


namespace wasabi::core {

struct MauseClickEvent;
struct OnCloseEvent;

struct EventVisitor {
	virtual void visit(MauseClickEvent& event) {};
	virtual void visit(OnCloseEvent& event) {};
};

} // namespace wasabi::core
