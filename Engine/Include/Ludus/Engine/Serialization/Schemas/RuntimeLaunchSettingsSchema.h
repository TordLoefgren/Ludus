#pragma once

#include <string>
#include <variant>

#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Runtime/RuntimeLaunchSettings.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamReader.h>
#include <Ludus/Engine/Serialization/Core/ITokenStreamWriter.h>
#include <Ludus/Engine/Serialization/Core/SerializationException.h>
#include <Ludus/Engine/Serialization/Core/TokenRead.h>

namespace Ludus::Engine::Serialization::Schemas
{
	using RuntimeLaunchSettings = Ludus::Engine::Runtime::RuntimeLaunchSettings;
	using ITokenStreamWriter = Ludus::Engine::Serialization::Core::ITokenStreamWriter;
	using ITokenStreamReader = Ludus::Engine::Serialization::Core::ITokenStreamReader;
	using SerializationException = Ludus::Engine::Serialization::Core::SerializationException;
	using Token = Ludus::Engine::Serialization::Core::Token;

	struct RuntimeLaunchSettingsSchema
	{
		inline static void Serialize(ITokenStreamWriter& writer, const RuntimeLaunchSettings& runtimeLaunchSettings)
		{
			writer.Emit(Token::StartObject { });

			{
				writer.Emit(Token::Key { "WindowSettings" });

				writer.Emit(Token::StartObject { });

				writer.Emit(Token::Key { "StartupWidth" });
				writer.Emit(Token::Int { runtimeLaunchSettings.WindowSettings.StartupWidth });

				writer.Emit(Token::Key { "StartupHeight" });
				writer.Emit(Token::Int { runtimeLaunchSettings.WindowSettings.StartupHeight });

				writer.Emit(Token::Key { "IsResizeable" });
				writer.Emit(Token::Bool { runtimeLaunchSettings.WindowSettings.IsResizeable });

				writer.Emit(Token::Key { "IsMaximized" });
				writer.Emit(Token::Bool { runtimeLaunchSettings.WindowSettings.IsMaximized });

				writer.Emit(Token::EndObject { });
			}

			{
				writer.Emit(Token::Key { "PresentationSettings" });

				writer.Emit(Token::StartObject { });

				writer.Emit(Token::Key { "InternalResolution" });
				writer.Emit(Token::StartObject { });
				writer.Emit(Token::Key { "Width" });
				writer.Emit(Token::Int { runtimeLaunchSettings.PresentationSettings.InternalResolution.Width });
				writer.Emit(Token::Key { "Height" });
				writer.Emit(Token::Int { runtimeLaunchSettings.PresentationSettings.InternalResolution.Height });
				writer.Emit(Token::EndObject { });

				writer.Emit(Token::Key { "UseFixedRenderResolution" });
				writer.Emit(Token::Bool { runtimeLaunchSettings.PresentationSettings.UseFixedRenderResolution });

				writer.Emit(Token::EndObject { });
			}

			writer.Emit(Token::EndObject { });
		}

		inline static Ludus::Engine::Core::Expected<RuntimeLaunchSettings, SerializationException> Deserialize(ITokenStreamReader& reader)
		{
			try
			{
				RuntimeLaunchSettings runtimeLaunchSettings;

				Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view key)
				{
					if (key == "WindowSettings")
					{
						Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view key)
						{
							if (key == "StartupWidth")
							{
								runtimeLaunchSettings.WindowSettings.StartupWidth = Ludus::Engine::Serialization::Core::ConsumeIntLike(reader);
								return;
							}
							if (key == "StartupHeight")
							{
								runtimeLaunchSettings.WindowSettings.StartupHeight = Ludus::Engine::Serialization::Core::ConsumeIntLike(reader);
								return;
							}
							if (key == "IsResizeable")
							{
								runtimeLaunchSettings.WindowSettings.IsResizeable = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Bool>(reader).Data;
								return;
							}
							if (key == "IsMaximized")
							{
								runtimeLaunchSettings.WindowSettings.IsMaximized = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Bool>(reader).Data;
								return;
							}

							Ludus::Engine::Serialization::Core::SkipValue(reader);
						});

						return;
					}

					if (key == "PresentationSettings")
					{
						Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view key)
						{
							if (key == "InternalResolution")
							{
								Ludus::Engine::Serialization::Core::ReadObject(reader, [&](std::string_view key)
								{
									if (key == "Width")
									{
										runtimeLaunchSettings.PresentationSettings.InternalResolution.Width = Ludus::Engine::Serialization::Core::ConsumeIntLike(reader);
										return;
									}
									if (key == "Height")
									{
										runtimeLaunchSettings.PresentationSettings.InternalResolution.Height = Ludus::Engine::Serialization::Core::ConsumeIntLike(reader);
										return;
									}

									Ludus::Engine::Serialization::Core::SkipValue(reader);
								});

								return;
							}
							if (key == "UseFixedRenderResolution")
							{
								runtimeLaunchSettings.PresentationSettings.UseFixedRenderResolution = Ludus::Engine::Serialization::Core::ConsumeAs<Token::Bool>(reader).Data;
								return;
							}

							Ludus::Engine::Serialization::Core::SkipValue(reader);
						});

						return;
					}

					Ludus::Engine::Serialization::Core::SkipValue(reader);
				});

				return runtimeLaunchSettings;
			}
			catch (const SerializationException& ex)
			{
				const auto error = Ludus::Engine::Serialization::Core::WithContext(
					ex, "RuntimeLaunchSettingsSchema::Deserialize"
				);

				return Ludus::Engine::Core::Unexpected<SerializationException>::Create(error);
			}
		}

	};
}
