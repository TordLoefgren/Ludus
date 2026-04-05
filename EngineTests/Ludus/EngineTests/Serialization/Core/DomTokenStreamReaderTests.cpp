#include "pch.h"

#include <variant>

#include <Ludus/Engine/Serialization/Core/DomDocument.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/DomTokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/Token.h>

namespace Ludus::EngineTests::Serialization::Core
{
	using DomDocument = Ludus::Engine::Serialization::Core::DomDocument;
	using DomTokenStreamReader = Ludus::Engine::Serialization::Core::DomTokenStreamReader;
	using DomTokenStreamWriter = Ludus::Engine::Serialization::Core::DomTokenStreamWriter;
	using Token = Ludus::Engine::Serialization::Core::Token;

	TEST(DomTokenStreamReader, Peek_IsIdempotent_And_ConsumeAdvances)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "a" });
		writer.Emit(Token::Int { 1 });
		writer.Emit(Token::Key { "b" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::Bool { true });
		writer.Emit(Token::String { "x" });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::EndObject { });

		DomTokenStreamReader reader(document);

		// Act.
		const auto& firstPeek = reader.Peek();
		const auto& secondPeek = reader.Peek();

		// Assert.
		ASSERT_TRUE(std::holds_alternative<Token::StartObject>(firstPeek.Data));
		ASSERT_TRUE(std::holds_alternative<Token::StartObject>(secondPeek.Data));

		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::Key>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::Int>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::Key>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::StartArray>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::Bool>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::String>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::EndArray>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::EndObject>(reader.Peek().Data));
		reader.Consume();

		ASSERT_TRUE(reader.IsComplete());
	}

	TEST(DomTokenStreamReader, ReadsArrayRootInOrder)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::Int { 1 });
		writer.Emit(Token::Int { 2 });
		writer.Emit(Token::EndArray { });

		DomTokenStreamReader reader(document);

		// Act.
		ASSERT_TRUE(std::holds_alternative<Token::StartArray>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::Int>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::Int>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::EndArray>(reader.Peek().Data));
		reader.Consume();

		// Assert.
		ASSERT_TRUE(reader.IsComplete());
	}

	TEST(DomTokenStreamReader, ReadsNestedObjectsAndArraysInOrder)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "items" });
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "id" });
		writer.Emit(Token::Int { 7 });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::Key { "id" });
		writer.Emit(Token::Int { 8 });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::EndArray { });
		writer.Emit(Token::Key { "empty" });
		writer.Emit(Token::StartObject { });
		writer.Emit(Token::EndObject { });
		writer.Emit(Token::EndObject { });

		DomTokenStreamReader reader(document);

		// Act.
		ASSERT_TRUE(std::holds_alternative<Token::StartObject>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::Key>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::StartArray>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::StartObject>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::Key>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::Int>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::EndObject>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::StartObject>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::Key>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::Int>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::EndObject>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::EndArray>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::Key>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::StartObject>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::EndObject>(reader.Peek().Data));
		reader.Consume();
		ASSERT_TRUE(std::holds_alternative<Token::EndObject>(reader.Peek().Data));
		reader.Consume();

		// Assert.
		ASSERT_TRUE(reader.IsComplete());
	}

#if LUDUS_ENABLE_ASSERTS
	TEST(DomTokenStreamReader, Construct_Dies_When_DocumentIsEmpty)
	{
		// Arrange.
		DomDocument document;

		// Act & Assert.
		ASSERT_DEATH({ DomTokenStreamReader reader(document); }, R"(The DOM document is empty\.)");
	}

	TEST(DomTokenStreamReader, Peek_Dies_When_ReaderIsComplete)
	{
		// Arrange.
		DomDocument document;
		DomTokenStreamWriter writer(document);
		writer.Emit(Token::StartArray { });
		writer.Emit(Token::EndArray { });

		DomTokenStreamReader reader(document);

		reader.Consume();
		reader.Consume();
		ASSERT_TRUE(reader.IsComplete());

		// Act & Assert.
		ASSERT_DEATH({ reader.Peek(); }, R"(Peek cannot be called on a completed DOM document\.)");
	}
#endif
}
