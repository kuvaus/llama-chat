#include "./header.h"

#include "../llm/llmodel.h"
#include "../llm/llamamodel.h"
#include "./utils.h"
#include "./parse_json.h"


//////////////////////////////////////////////////////////////////////////
////////////                    ANIMATION                     ////////////
//////////////////////////////////////////////////////////////////////////

std::atomic<bool> stop_display{false}; 

void display_frames() {
    const char* frames[] = {".", ":", "'", ":"};
    int frame_index = 0;
    ConsoleState con_st;
    con_st.use_color = true;
    while (!stop_display) {
        set_console_color(con_st, PROMPT);
        std::cout << "\r" << frames[frame_index % 4] << std::flush;
        frame_index++;
        set_console_color(con_st, DEFAULT);
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    std::cout << "\r" << " " << std::flush;
    std::cout << "\r" << std::flush;
}

void display_loading() {

    while (!stop_display) {


        for (int i=0; i < 10; i++){
                fprintf(stdout, ".");
                fflush(stdout);
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                if (stop_display){ break; }
        }
        
            std::cout << "\r" << "           " << "\r" << std::flush;
    }
    std::cout << "\r" << " " << std::flush;

}


//////////////////////////////////////////////////////////////////////////
////////////                    ANIMATION                     ////////////
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
////////////                 LLAMA FUNCTIONS                  ////////////
//////////////////////////////////////////////////////////////////////////

void update_struct(LLModel::PromptContext  &prompt_context, GPTJParams &params){
    // TODO: handle this better
    prompt_context.n_predict = params.n_predict;
    prompt_context.top_k = params.top_k;
    prompt_context.top_p = params.top_p;
    prompt_context.temp = params.temp;
    prompt_context.n_batch = params.n_batch;

    prompt_context.repeat_penalty = 1.10f;  
    prompt_context.repeat_last_n = 64;  
    prompt_context.contextErase = 0.75f; 
    }
    
void printPromptContext(LLModel::PromptContext& context) {
    // Print n_past
    std::cout << "n_past: " << context.n_past << std::endl;

    // Print logits
    std::cout << "logits: ";
    for (const auto& logit : context.logits) {
        std::cout << logit << " ";
    }
    std::cout << std::endl;
}

//////////////////////////////////////////////////////////////////////////
////////////                 LLAMA FUNCTIONS                  ////////////
//////////////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////////////
////////////                 CHAT FUNCTIONS                   ////////////
//////////////////////////////////////////////////////////////////////////



std::string get_input(ConsoleState& con_st, LLamaModel llama_model, std::string& input) {
    set_console_color(con_st, USER_INPUT);

    std::cout << "\n> ";
    std::getline(std::cin, input);
    set_console_color(con_st, DEFAULT);

    if (input == "exit" || input == "quit") {
        //free_model(llama_model);
        llama_model.~LLamaModel();
        exit(0);
    }

    return input;
}


//////////////////////////////////////////////////////////////////////////
////////////                 CHAT FUNCTIONS                   ////////////
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
////////////                  MAIN PROGRAM                    ////////////
//////////////////////////////////////////////////////////////////////////


int main(int argc, char* argv[]) {


    ConsoleState con_st;
    con_st.use_color = true;
    set_console_color(con_st, DEFAULT);

    bool interactive = true;
    bool continuous = true;

    std::string response;
    response.reserve(10000);
    int memory = 200;
    GPTJParams params;
    std::string prompt = "";
    std::string input = "";
    std::string answer = "";
   

    set_console_color(con_st, PROMPT);
    set_console_color(con_st, BOLD);
    std::cout << "llama-chat";
    set_console_color(con_st, DEFAULT);
    std::cout << "" << std::endl;
    
    parse_params(argc, argv, params, prompt, interactive, continuous, memory);



    bool use_animation = true;


    LLamaModel llama_model;

    auto future = std::async(std::launch::async, display_loading);

    //handle stderr for now
    #ifdef _WIN32
	    int nullfd = _open("NUL", _O_WRONLY);
	    _dup2(nullfd, _fileno(stderr));
	    _close(nullfd);
    #else
        int stderr_copy = dup(fileno(stderr));
        std::freopen("/dev/null", "w", stderr);
     #endif

    std::cout << "\r" << "llama-chat: loading " << params.model.c_str()  << std::endl;
    auto check_llama = llama_model.loadModel( params.model.c_str() );

    //bring back stderr for now
    #ifdef _WIN32
    	// currently not handed
    #else
        dup2(stderr_copy, fileno(stderr));
        close(stderr_copy);
    #endif

    if (check_llama == false) {
        stop_display = true;
        future.wait();
        stop_display= false;   

        std::cerr << "Error loading: " << params.model.c_str() << std::endl;
        std::cout << "Press any key to exit..." << std::endl;
        std::cin.get();
        return 0;
    } else {
        stop_display = true;
        future.wait();
        stop_display= false;
        std::cout << "\r" << "llama-chat: done loading!" << std::flush;   
    }

    set_console_color(con_st, PROMPT);
    std::cout << " " << prompt.c_str() << std::endl;
    set_console_color(con_st, DEFAULT);

    std::string default_prefix = "### Instruction:\n The prompt below is a question to answer, a task to complete, or a conversation to respond to; decide which and write an appropriate response.";
    std::string default_header = "\n### Prompt: ";
    std::string default_footer = "\n### Response: ";
  
  
  
    //////////////////////////////////////////////////////////////////////////
    ////////////            PROMPT LAMBDA FUNCTIONS               ////////////
    //////////////////////////////////////////////////////////////////////////



	auto lambda_prompt = [](int32_t token_id) {
	        // You can handle prompt here if needed
	        //std::cout << token_id << std::flush;
	        //std::cout << promptchars << std::flush;
	        return true;
	    };
	    
	std::string hashstring ="";
	auto lambda_response = [&hashstring, &answer](int32_t token_id, std::string response) {
	   		 
	
	        if (!response.empty()) {
	        // stop the animation, printing response
	        stop_display = true;
	
	        // handle ### token separately
	        if (response == "#" || response == "##") {
	            hashstring += response;
	        } else if (response == "###" || hashstring == "###") {
	            hashstring = "";
	            return false;
	        }
				std::cout << response << std::flush;
	            answer = answer + response;
	        }
	            
	    return true;
	};
	
	auto lambda_recalculate = [](bool is_recalculating) {
	        // You can handle recalculation requests here if needed
	    return true;
	};


    //////////////////////////////////////////////////////////////////////////
    ////////////            PROMPT LAMBDA FUNCTIONS               ////////////
    //////////////////////////////////////////////////////////////////////////

  	//create PromptContext and update fields from params struct.
    LLModel::PromptContext  prompt_context;
    update_struct(prompt_context, params);
    
    
    llama_model.setThreadCount(params.n_threads);

    if (interactive) {
        input = get_input(con_st, llama_model, input);
        if (prompt != "") {
            if (use_animation){ future = std::async(std::launch::async, display_frames); }
            llama_model.prompt((default_prefix + default_header + prompt + " " + input + default_footer).c_str(),
            lambda_prompt, lambda_response, lambda_recalculate, prompt_context);
            if (use_animation){ stop_display = true; future.wait(); stop_display = false; }
        } else {
            if (use_animation){ future = std::async(std::launch::async, display_frames); }
            llama_model.prompt((default_prefix + default_header + input + default_footer).c_str(),
            lambda_prompt, lambda_response, lambda_recalculate, prompt_context);
            if (use_animation){ stop_display = true; future.wait(); stop_display = false; }

        }
        //answer = response.c_str();

        while (continuous) {
            std::string memory_string = default_prefix;
            if (memory > 1) {
                memory_string = default_prefix + default_header + input.substr(0, memory) + default_footer + answer.substr(0, memory);
            }
            answer = ""; //New prompt. We stored previous answer in memory so clear it.
            input = get_input(con_st, llama_model, input);
            if (use_animation){ future = std::async(std::launch::async, display_frames); }
            llama_model.prompt((memory_string + default_header + input + default_footer).c_str(), 
            lambda_prompt, lambda_response, lambda_recalculate, prompt_context);
            if (use_animation){ stop_display = true; future.wait(); stop_display = false; }

            //answer = response.c_str();
        }
    } else {
        if (use_animation){ future = std::async(std::launch::async, display_frames); }
        llama_model.prompt((default_prefix + default_header + prompt + default_footer).c_str(), 
        lambda_prompt, lambda_response, lambda_recalculate, prompt_context);
        if (use_animation){ stop_display = true; future.wait(); stop_display = false; }

    }




    set_console_color(con_st, DEFAULT);
    printPromptContext(prompt_context);
    llama_model.~LLamaModel();


    return 0;
}
