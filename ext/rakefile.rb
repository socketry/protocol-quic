# frozen_string_literal: true

task :default do
	ruby_library_directory = ENV.fetch("RUBYLIBDIR"){ENV.fetch("RUBYARCHDIR")}
	build_environment = {"RUBYLIBDIR" => ruby_library_directory}
	
	sh build_environment, "teapot", "fetch"
	sh build_environment, "teapot", "Ruby/Protocol/QUIC"
end
